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

bool RadixGraph::Insert(DummyNode* src, int des, float weight, int delta_deg) {
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
            auto new_array = new WeightedEdgeArray(deg * 2 + (next->size - next->cap + 1) + 8); // +(next->size -next->cap + 1) to avoid repititive expanding (especially the updates are intensive), +8 to avoid empty edge array
            new_array = LogCompaction(next, new_array);
            new_array->prev_arr = next;
            next->next_arr = new_array;
            src->next.store(new_array);
            if (next->threads_analytical.load() == 0 && next->threads_get_neighbor.load() == 0) {
                if (next->edge) delete [] next->edge, next->edge = nullptr; // Do not delete next pointer to avoid concurrency issue
                if (next->prev_arr) {
                    auto tmp = next->prev_arr;
                    if (tmp->threads_analytical.load() == 0 && tmp->threads_get_neighbor.load() == 0) {
                        delete tmp; // But you can delete the previous version (snapshot) safely
                    }
                }
            }
        }
        next = src->next.load();
        i = next->size.fetch_add(1);
        src->mtx.clear();
    }
    next->deg.fetch_add(delta_deg);
    next->edge[i] = {weight, des};
    next->physical_size.fetch_add(1);
    return true;
}

bool RadixGraph::InsertEdge(NodeID src, NodeID des, float weight) {
    SORT::global_timestamp++;
    DummyNode* src_ptr = vertex_index->RetrieveVertex(src, true);
    DummyNode* des_ptr = vertex_index->RetrieveVertex(des, true);
    Insert(src_ptr, des_ptr->idx, weight, 1);
    return true;
}

bool RadixGraph::UpdateEdge(NodeID src, NodeID des, float weight) {
    SORT::global_timestamp++;
    DummyNode* src_ptr = vertex_index->RetrieveVertex(src);
    if (!src_ptr || src_ptr->del_time != -1) {
        // Vertex not exist or deleted
        return false;
    }
    DummyNode* des_ptr = vertex_index->RetrieveVertex(des);
    if (!des_ptr || des_ptr->del_time != -1) {
        // Vertex not exist or deleted
        return false;
    }
    Insert(src_ptr, des_ptr->idx, weight, 0);
    return true;
}

bool RadixGraph::DeleteEdge(NodeID src, NodeID des) {
    SORT::global_timestamp++;
    DummyNode* src_ptr = vertex_index->RetrieveVertex(src);
    if (!src_ptr || src_ptr->del_time != -1) {
        // Vertex not exist or deleted
        return false;
    }
    DummyNode* des_ptr = vertex_index->RetrieveVertex(des);
    if (!des_ptr || des_ptr->del_time != -1) {
        // Vertex not exist or deleted
        return false;
    }
    Insert(src_ptr, des_ptr->idx, 0, -1);
    return true;
}

bool RadixGraph::GetNeighbours(NodeID src, std::vector<WeightedEdge> &neighbours, bool is_snapshot, int timestamp) {
    SORT::global_timestamp++;
    DummyNode* src_ptr = vertex_index->RetrieveVertex(src);
    if (!src_ptr || (src_ptr->del_time != -1 && timestamp >= src_ptr->del_time)) {
        // Vertex not exist or deleted
        return false;
    }
    return GetNeighbours(src_ptr, neighbours, is_snapshot);
}

bool RadixGraph::GetNeighboursByOffset(int src, std::vector<WeightedEdge> &neighbours, bool is_snapshot, int timestamp) {
    SORT::global_timestamp++;
    if (src >= vertex_index->cnt) {
        // Vertex not exist
        return false;
    }
    auto src_ptr = &vertex_index->vertex_table[src];
    if (src_ptr->del_time != -1 && timestamp >= src_ptr->del_time) {
        // Vertex deleted
        return false;
    }
    return GetNeighbours(src_ptr, neighbours, is_snapshot);
}

bool RadixGraph::GetNeighbours(DummyNode* src, std::vector<WeightedEdge> &neighbours, bool is_snapshot, int timestamp) {
    SORT::global_timestamp++;
    auto next = src->next.load();
    if (is_snapshot) {
        // Get corresponding snapshot for read
        while (next && (next->snapshot_timestamp > timestamp || next->edge == nullptr)) {
            next = next->prev_arr;
        }
        if (!next) {
            // Not a valid timestamp
            return true;
        }
    }
    next->threads_get_neighbor.fetch_add(1);
    int cnt = is_snapshot ? next->snapshot_deg.load() : next->physical_size.load();
    if (!is_snapshot) {
        // Full neighbor list
        int thread_id = thread_id_local == -1 ? omp_get_thread_num() : thread_id_local;
        for (int i = cnt - 1; i >= 0; i--) {
            auto& e = next->edge[i];
            // Avoid read-write conflicts: check e.idx first
            if (e.idx != -1 && !bitmap[thread_id]->get_bit(e.idx)) {
                if (e.weight != 0) { // Insert or Update
                    // Have not found a previous log for this edge, thus this edge is the latest
                    neighbours.emplace_back(e);
                }
                if (i >= next->snapshot_deg) bitmap[thread_id]->set_bit(e.idx);
            }
        }
        for (int i = next->snapshot_deg; i < cnt; i++) {
            if (next->edge[i].idx != -1) bitmap[thread_id]->clear_bit(next->edge[i].idx);
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
    next->threads_get_neighbor.fetch_sub(1);
    if (next->threads_analytical.load() == 0 && next->threads_get_neighbor.load() == 0 && next != src->next.load()) {
        // If array pointer changes, this array has been obseleted, and if no other querying process, then delete this array safely;
        delete next;
    }

    return true;
}

WeightedEdgeArray* RadixGraph::LogCompaction(WeightedEdgeArray* old_arr, WeightedEdgeArray* new_arr) {
    int thread_id = thread_id_local == -1 ? omp_get_thread_num() : thread_id_local;
    int num = 0;
    // Full log compaction, used in graph updates
    for (int i = old_arr->cap - 1; i >= 0; i--) {
        auto& e = old_arr->edge[i];
        // Optional: check whether the destination vertex is deleted;
        // Here we defer the check process to get_neighbors (will return false if the vertex is deleted)
        if (!bitmap[thread_id]->get_bit(e.idx)) {
            if (e.weight != 0) { // Insert or Update
                // Have not found a previous log for this edge, thus this edge is the latest
                if (num >= new_arr->cap) {
                    // Normally should not happen. In case for invalid operations like duplicate edges, deleting or updating non-existing edges.
                    auto tmp = new_arr;
                    new_arr = new WeightedEdgeArray(num * 2);
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
    new_arr->size.store(num);
    new_arr->snapshot_deg.store(num);
    new_arr->deg.store(num);
    new_arr->physical_size.store(num);
    new_arr->snapshot_timestamp = SORT::global_timestamp.load();
    for (int i = old_arr->snapshot_deg; i < old_arr->cap; i++) {
        bitmap[thread_id]->clear_bit(old_arr->edge[i].idx);
    }

    return new_arr;
}

std::vector<uint64_t> RadixGraph::BFS(NodeID src) {
    std::queue<int> Q;
    AtomicBitmap vis(vertex_index->cnt);
    vis.reset();
    auto src_ptr = vertex_index->RetrieveVertex(src);
    vis.set_bit(src_ptr->idx);
    Q.push(src_ptr->idx);
    std::vector<uint64_t> res;
    while (!Q.empty()) {
        auto u = Q.front();
        Q.pop();
        res.push_back(vertex_index->vertex_table[u].node);
        std::vector<WeightedEdge> neighbours;
        GetNeighboursByOffset(u, neighbours);
        for (auto e : neighbours) {
            if (!vis.get_bit(e.idx)) {
                vis.set_bit(e.idx);
                Q.push(e.idx);
            }
        }
    }
    return res;
}

std::vector<double> RadixGraph::SSSP(NodeID src) {
    std::vector<double> dist;
    dist.assign(vertex_index->cnt, 1e9);
    auto u = vertex_index->RetrieveVertex(src);
    std::priority_queue<std::pair<double, int>> Q;
    dist[u->idx] = 0;
    Q.emplace(0, u->idx);
    while (!Q.empty()) {
        auto v = Q.top().second;
        Q.pop();
        std::vector<WeightedEdge> neighbours;
        GetNeighboursByOffset(v, neighbours);
        for (auto e : neighbours) {
            auto w = e.idx;
            if (dist[v] + e.weight < dist[w]) {
                dist[w] = dist[v] + e.weight;
                Q.emplace(-dist[w], w);
            }
        }
    }
    return dist;
}

RadixGraph::RadixGraph(int d, std::vector<int> _num_children) {
    bitmap = new AtomicBitmap*[max_number_of_threads];
    for (int i = 0; i < max_number_of_threads; i++) bitmap[i] = new AtomicBitmap(CAP_DUMMY_NODES), bitmap[i]->reset();
    vertex_index = new SORT(d, _num_children);
}

RadixGraph::~RadixGraph() {
    if (bitmap) {
        for (int i = 0; i < max_number_of_threads; i++) delete bitmap[i];
        delete [] bitmap;
    }
    delete vertex_index;
}