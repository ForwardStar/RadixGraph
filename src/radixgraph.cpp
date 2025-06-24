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

bool RadixGraph::Insert(DummyNode* src, DummyNode* des, double weight) {
    while (src->mtx.test_and_set()) {}
    if (src->size.load() >= src->cap.load()) {
        auto old_array = src->next.load();
        auto new_array = new WeightedEdgeArray(src->cap * 1.5);
        for (int i = 0; i < src->cap; i++) {
            new_array->edge[i] = old_array->edge[i];
        }
        src->next.store(new_array);
        src->cap.store(src->cap.load() * 1.5);
        if (old_array->threads_analytical.load() == 0 && old_array->threads_get_neighbor.load() == 0) {
            delete old_array;
        }
    }
    int i = src->size.fetch_add(1);
    src->next.load()->edge[i].weight = weight;
    src->next.load()->edge[i].idx = des->idx;
    src->mtx.clear();
    return true;
}

bool RadixGraph::InsertEdge(NodeID src, NodeID des, double weight) {
    DummyNode* src_ptr = vertex_index->RetrieveVertex(src, true);
    DummyNode* des_ptr = vertex_index->RetrieveVertex(des, true);
    src_ptr->deg.fetch_add(1);
    Insert(src_ptr, des_ptr, weight);
    return true;
}

bool RadixGraph::UpdateEdge(NodeID src, NodeID des, double weight) {
    DummyNode* src_ptr = vertex_index->RetrieveVertex(src);
    if (!src_ptr) {
        return false;
    }
    DummyNode* des_ptr = vertex_index->RetrieveVertex(des);
    if (!des_ptr) {
        return false;
    }
    Insert(src_ptr, des_ptr, weight);
    return true;
}

bool RadixGraph::DeleteEdge(NodeID src, NodeID des) {
    DummyNode* src_ptr = vertex_index->RetrieveVertex(src);
    if (!src_ptr) {
        return false;
    }
    DummyNode* des_ptr = vertex_index->RetrieveVertex(des);
    if (!des_ptr) {
        return false;
    }
    src_ptr->deg.fetch_sub(1);
    Insert(src_ptr, des_ptr, 0);
    return true;
}

bool RadixGraph::GetNeighbours(NodeID src, std::vector<WeightedEdge> &neighbours, int timestamp) {
    DummyNode* src_ptr = vertex_index->RetrieveVertex(src);
    if (!src_ptr) {
        return false;
    }
    return GetNeighbours(src_ptr, neighbours, timestamp);
}

bool RadixGraph::GetNeighbours(DummyNode* src, std::vector<WeightedEdge> &neighbours, int timestamp) {
    int thread_id = omp_get_thread_num(), cnt = timestamp == -1 ? src->size.load() : timestamp;
    auto next = src->next.load();
    next->threads_get_neighbor.fetch_add(1);
    for (int i = cnt - 1; i >= 0; i--) {
        auto& e = next->edge[i];
        // Avoid read-write conflicts: check e.idx first
        if (e.idx != -1 && !bitmap[thread_id]->get_bit(e.idx)) {
            if (e.weight != 0) { // Insert or Update
                // Have not found a previous log for this edge, thus this edge is the latest
                neighbours.emplace_back(e);
            }
            bitmap[thread_id]->set_bit(e.idx);
        }
    }
    for (int i = 0; i < cnt; i++) {
        if (next->edge[i].idx != -1) bitmap[thread_id]->clear_bit(next->edge[i].idx);
    }
    next->threads_get_neighbor.fetch_sub(1);
    if (next->threads_analytical.load() == 0 && next->threads_get_neighbor.load() == 0 && next != src->next.load()) {
        // If array pointer changes, this array has been obseleted, and if no other querying process, then delete this array;
        delete next;
    }

    return true;
}

bool RadixGraph::GetNeighboursByOffset(int src, std::vector<WeightedEdge> &neighbours, int timestamp) {
    auto& src_ptr = vertex_index->vertex_table[src];
    int thread_id = omp_get_thread_num(), cnt = timestamp == -1 ? src_ptr.size.load() : timestamp;
    auto next = src_ptr.next.load();
    next->threads_get_neighbor.fetch_add(1);
    for (int i = cnt - 1; i >= 0; i--) {
        auto& e = next->edge[i];
        // Avoid read-write conflicts: check e.idx first
        if (e.idx != -1 && !bitmap[thread_id]->get_bit(e.idx)) {
            if (e.weight != 0) { // Insert or Update
                // Have not found a previous log for this edge, thus this edge is the latest
                neighbours.emplace_back(e);
            }
            bitmap[thread_id]->set_bit(e.idx);
        }
    }
    for (int i = 0; i < cnt; i++) {
        if (next->edge[i].idx != -1) bitmap[thread_id]->clear_bit(next->edge[i].idx);
    }
    next->threads_get_neighbor.fetch_sub(1);
    if (next->threads_analytical.load() == 0 && next->threads_get_neighbor.load() == 0 && next != src_ptr.next.load()) {
        // If array pointer changes, this array has been obseleted, and if no other querying process, then delete this array;
        delete next;
    }

    return true;
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