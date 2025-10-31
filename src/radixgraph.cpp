/*
 * Copyright (C) 2025 Haoxuan Xie
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "radixgraph.h"

thread_local int RadixGraph::thread_id_local = -1;

bool RadixGraph::Insert(Vertex* src, int des, float weight, int delta_deg) {
    auto next = src->next.load();
    int i = next->size.fetch_add(1);
    while (i >= next->cap) {
        while (src->mtx.test_and_set()) {}
        if (next == src->next.load()) {
            // Array not expanded, start expanding
            while (next->physical_size.load() != next->cap) {
                // Wait for all edges to be written
            }
            auto deg = next->deg.load();
            deg = std::max(deg, 0); // Deleting non-existing edges may lead to negative degree (although this should not happen)
            auto new_array = new WeightedEdgeArray(deg * expand_rate + (next->size - next->cap + 1) + 8); // +(next->size -next->cap + 1) to avoid repititive expanding (especially the updates are intensive), +8 to avoid empty edge array
            new_array = LogCompaction(next, new_array);
            new_array->prev_arr = next;
            next->next_arr = new_array;
            src->next.store(new_array);
            if (next->threads_read == 0) {
                if (next->edge) delete [] next->edge, next->edge = nullptr; // Do not delete next pointer to avoid concurrency issue
                if (next->timestamp) delete [] next->timestamp, next->timestamp = nullptr;
                if (next->prev_arr) {
                    auto tmp = next->prev_arr;
                    if (tmp->threads_read == 0) {
                        delete tmp; // But you can delete the previous version (snapshot) safely
                    }
                }
            }
        }
        next = src->next.load();
        i = next->size.fetch_add(1);
        src->mtx.clear();
    }
    if (next->timestamp) {
        next->timestamp[i - next->snapshot_deg] = GetGlobalTimestamp();
    }
    next->deg.fetch_add(delta_deg);
    next->edge[i] = {weight, des};
    next->physical_size.fetch_add(1);
    return true;
}

bool RadixGraph::InsertEdge(NodeID src, NodeID des, float weight) {
    Vertex* src_ptr = vertex_index->RetrieveVertex(src, true);
    Vertex* des_ptr = vertex_index->RetrieveVertex(des, true);
    Insert(src_ptr, des_ptr->idx, weight, 1);
    return true;
}

bool RadixGraph::UpdateEdge(NodeID src, NodeID des, float weight) {
    Vertex* src_ptr = vertex_index->RetrieveVertex(src);
    if (!src_ptr || src_ptr->del_time != -1) {
        // Vertex not exist or deleted
        return false;
    }
    Vertex* des_ptr = vertex_index->RetrieveVertex(des);
    if (!des_ptr || des_ptr->del_time != -1) {
        // Vertex not exist or deleted
        return false;
    }
    Insert(src_ptr, des_ptr->idx, weight, 0);
    return true;
}

bool RadixGraph::DeleteEdge(NodeID src, NodeID des) {
    Vertex* src_ptr = vertex_index->RetrieveVertex(src);
    if (!src_ptr || src_ptr->del_time != -1) {
        // Vertex not exist or deleted
        return false;
    }
    Vertex* des_ptr = vertex_index->RetrieveVertex(des);
    if (!des_ptr || des_ptr->del_time != -1) {
        // Vertex not exist or deleted
        return false;
    }
    Insert(src_ptr, des_ptr->idx, 0, -1);
    return true;
}

bool RadixGraph::GetNeighbours(NodeID src, std::vector<WeightedEdge> &neighbours, int timestamp) {
    Vertex* src_ptr = vertex_index->RetrieveVertex(src);
    if (!src_ptr || (src_ptr->del_time != -1 && timestamp >= src_ptr->del_time)) {
        // Vertex not exist or deleted
        return false;
    }
    return GetNeighbours(src_ptr, neighbours, timestamp);
}

bool RadixGraph::GetNeighboursByOffset(int src, std::vector<WeightedEdge> &neighbours, int timestamp) {
    if (src >= vertex_index->cnt) {
        // Vertex not exist
        return false;
    }
    auto src_ptr = &vertex_index->vertex_table[src];
    if (src_ptr->del_time != -1 && timestamp >= src_ptr->del_time) {
        // Vertex deleted
        return false;
    }
    return GetNeighbours(src_ptr, neighbours, timestamp);
}

bool RadixGraph::GetNeighbours(Vertex* src, std::vector<WeightedEdge> &neighbours, int timestamp) {
    if (global_info.is_mixed_workloads) {
        // Before reference counter is updated, no log compactions should be performed
        while (src->mtx.test_and_set()) {}
    }
    auto next = src->next.load();
    // Get corresponding snapshot for read
    while (next && (next->snapshot_timestamp > timestamp || next->edge == nullptr)) {
        next = next->prev_arr;
    }
    if (!next) {
        // Not a valid timestamp
        if (global_info.is_mixed_workloads) src->mtx.clear();
        return false;
    }
    if (global_info.is_mixed_workloads) {
        next->threads_read++;
        src->mtx.clear();
    }
    int cnt = next->physical_size.load();
    if (cnt > next->snapshot_deg.load()) {
        // Full neighbor list
        int thread_id = thread_id_local == -1 ? omp_get_thread_num() : thread_id_local;
        if (next->timestamp) {
            for (int i = cnt - 1; i >= next->snapshot_deg; i--) {
                if (next->timestamp[i - next->snapshot_deg] <= timestamp) {
                    cnt = i + 1;
                    break;
                }
                if (i == next->snapshot_deg) {
                    // Only the snapshot is visible
                    cnt = next->snapshot_deg.load();
                    neighbours.resize(cnt);
                    for (int i = 0; i < cnt; i++) {
                        auto& e = next->edge[i];
                        neighbours[i] = e;
                    }
                    cnt = 0;
                }
            }
        }
        for (int i = cnt - 1; i >= 0; i--) {
            auto& e = next->edge[i];
            int retry_count = 0;
            while (e.idx == -1 && retry_count < 10000) {
                // Wait for edge to be written
                // Use retry_count to avoid deadlocks
                retry_count++;
            }
            if (e.idx != -1 && !bitmap[thread_id]->get_bit(e.idx)) {
                if (e.weight != 0) { // Insert or Update
                    // Have not found a previous log for this edge, thus this edge is the latest
                    neighbours.emplace_back(e);
                }
                if (i >= next->snapshot_deg) bitmap[thread_id]->set_bit(e.idx);
            }
        }
        for (int i = next->snapshot_deg; i < cnt; i++) {
            auto& e = next->edge[i];
            if (e.idx != -1) bitmap[thread_id]->clear_bit(e.idx);
        }
    }
    else {
        // Snapshot neighbor list
        neighbours.resize(cnt);
        for (int i = 0; i < cnt; i++) {
            auto& e = next->edge[i];
            neighbours[i] = e;
        }
    }
    if (global_info.is_mixed_workloads) {
        // Before reference counter is updated, no log compactions should be performed
        while (src->mtx.test_and_set()) {}
        next->threads_read--;
        if (next->threads_read == 0 && next != src->next.load()) {
            // If array pointer changes, this array has been obseleted, and if no other querying process, then delete this array safely;
            delete next;
        }
        src->mtx.clear();
    }

    return true;
}

WeightedEdgeArray* RadixGraph::LogCompaction(WeightedEdgeArray* old_arr, WeightedEdgeArray* new_arr) {
    int num = 0;
    if (old_arr->physical_size == old_arr->deg) {
        // All logs are insertions. Can merge without duplicate checker
        num = old_arr->physical_size;
        for (int i = 0; i < old_arr->physical_size; i++) {
            new_arr->edge[i] = old_arr->edge[i];
        }
    }
    else {
        int thread_id = thread_id_local == -1 ? omp_get_thread_num() : thread_id_local;
        for (int i = old_arr->physical_size - 1; i >= 0; i--) {
            auto& e = old_arr->edge[i];
            // Optional: check whether the destination vertex is deleted;
            // Here we defer the check process to get_neighbors (will return false if the vertex is deleted)
            if (!bitmap[thread_id]->get_bit(e.idx)) {
                if (e.weight != 0) { // Insert or Update
                    // Have not found a previous log for this edge, thus this edge is the latest
                    if (num >= new_arr->cap) {
                        // Normally should not happen. In case for invalid operations like duplicate edges, deleting or updating non-existing edges.
                        auto tmp = new_arr;
                        new_arr = new WeightedEdgeArray(num + 8);
                        for (int j = 0; j < num; j++) {
                            new_arr->edge[j] = tmp->edge[j];
                        }
                        delete tmp;
                    }
                    new_arr->edge[num++] = {e.weight, e.idx};
                }
                if (i >= old_arr->snapshot_deg) bitmap[thread_id]->set_bit(e.idx);
            }
        }
        for (int i = old_arr->snapshot_deg; i < old_arr->physical_size; i++) {
            auto& e = old_arr->edge[i];
            bitmap[thread_id]->clear_bit(old_arr->edge[i].idx);
        }
    }
    new_arr->size.store(num);
    new_arr->snapshot_deg.store(num);
    new_arr->deg.store(num);
    new_arr->physical_size.store(num);
    if (global_info.is_mixed_workloads) new_arr->timestamp = new int[new_arr->cap - num];
    new_arr->snapshot_timestamp = GetGlobalTimestamp();

    return new_arr;
}

int RadixGraph::GetInternalID(NodeID id) {
    Vertex* v = vertex_index->RetrieveVertex(id);
    if (!v || v->del_time != -1) {
        throw std::out_of_range("Vertex ID not found");
    }
    return v->idx;
}

NodeID RadixGraph::GetExternalID(int offset) {
    if (offset >= vertex_index->cnt) {
        throw std::out_of_range("Internal ID out of range");
    }
    return vertex_index->vertex_table[offset].node;
}

NodeID RadixGraph::GetExternalID(WeightedEdge& e) {
    int offset = e.idx;
    if (offset >= vertex_index->cnt) {
        throw std::out_of_range("Internal ID out of range");
    }
    return vertex_index->vertex_table[offset].node;
}

void RadixGraph::CreateSnapshots(bool sort_neighbours, bool make_dense) {
    // Should be executed when no updates are performed
    int n = vertex_index->cnt.load();
    #pragma omp parallel for num_threads(num_threads)
    for (int i = 0; i < n; i++) {
        auto src = &vertex_index->vertex_table[i];
        auto next = src->next.load();
        auto deg = next->deg.load();
        deg = std::max(deg, 0); // Deleting non-existing edges may lead to negative degree (although this should not happen)
        WeightedEdgeArray* new_array = nullptr;
        if (!make_dense) {
            new_array = new WeightedEdgeArray(deg * expand_rate + 8); // +8 to avoid empty edge array
        }
        else {
            new_array = new WeightedEdgeArray(deg);
        }
        new_array = LogCompaction(next, new_array);
        new_array->prev_arr = next;
        next->next_arr = new_array;
        src->next.store(new_array);
        if (sort_neighbours && new_array->physical_size.load() > 1) {
            std::sort(new_array->edge, new_array->edge + new_array->physical_size.load(), [](const WeightedEdge &a, const WeightedEdge &b) {
                return a.idx < b.idx;
            });
        }
        // Since CreateSnapshot() is exclusive, can delete previous versions safely
        delete next;
    }
    is_sorted = sort_neighbours;
}

int RadixGraph::GetGlobalTimestamp() {
    return global_info.global_timestamp++;
}

void RadixGraph::SetNumThreads(int nth) {
    if (num_threads != nth) {
        if (bitmap) {
            for (int i = 0; i < num_threads; i++) delete bitmap[i];
            delete [] bitmap;
        }
        num_threads = nth;
        bitmap = new SegmentedBitmap*[num_threads];
        for (int i = 0; i < num_threads; i++) bitmap[i] = new SegmentedBitmap(), bitmap[i]->reset();
    }
}

#if !USE_SORT && !USE_ART
    void RadixGraph::SetMaximumID(int max_id) {
        delete vertex_index;
        vertex_index = new VertexArray(max_id + 1);
    }
#endif

#if USE_SORT
    RadixGraph::RadixGraph(int n, int bit_length, int _num_threads, int L) {
        num_threads = _num_threads;
        bitmap = new SegmentedBitmap*[num_threads];
        for (int i = 0; i < num_threads; i++) bitmap[i] = new SegmentedBitmap(L), bitmap[i]->reset();
        int l = ceil(log2(bit_length)); // O(lglg(u)) layers
        if (l == 0) l = 1;
        vertex_index = new SORT(n, bit_length, l);
    }
    RadixGraph::RadixGraph(std::vector<int> _num_bits, int _num_threads, int L) {
        num_threads = _num_threads;
        bitmap = new SegmentedBitmap*[num_threads];
        for (int i = 0; i < num_threads; i++) bitmap[i] = new SegmentedBitmap(L), bitmap[i]->reset();
        vertex_index = new SORT(_num_bits);
    }
#elif USE_ART
    RadixGraph::RadixGraph(int _num_threads, int L) {
        num_threads = _num_threads;
        bitmap = new SegmentedBitmap*[num_threads];
        for (int i = 0; i < num_threads; i++) bitmap[i] = new SegmentedBitmap(L), bitmap[i]->reset();
        vertex_index = new ART();
    }
#else
    RadixGraph::RadixGraph(int _num_threads, int _max_vertex_id, int L) {
        num_threads = _num_threads;
        bitmap = new SegmentedBitmap*[num_threads];
        for (int i = 0; i < num_threads; i++) bitmap[i] = new SegmentedBitmap(L), bitmap[i]->reset();
        vertex_index = new VertexArray(_max_vertex_id + 1);
    }
#endif

RadixGraph::~RadixGraph() {
    if (bitmap) {
        for (int i = 0; i < num_threads; i++) delete bitmap[i];
        delete [] bitmap;
    }
    delete vertex_index;
}