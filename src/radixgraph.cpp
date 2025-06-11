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
    src->next.emplace_back(weight, des->idx);
    return true;
}

bool RadixGraph::InsertEdge(NodeID src, NodeID des, double weight) {
    DummyNode* src_ptr = vertex_index->RetrieveVertex(src, true);
    DummyNode* des_ptr = vertex_index->RetrieveVertex(des, true);
    src_ptr->deg.fetch_add(1);
    if (enable_query) degree[src_ptr->idx].fetch_add(1);
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
    if (enable_query) degree[src_ptr->idx].fetch_sub(1);
    Insert(src_ptr, des_ptr, 0);
    return true;
}

bool RadixGraph::GetNeighbours(NodeID src, std::vector<WeightedEdge> &neighbours, int timestamp) {
    DummyNode* src_ptr = vertex_index->RetrieveVertex(src);
    if (!src_ptr) {
        return false;
    }
    return GetNeighbours(src_ptr->idx, neighbours, timestamp);
}

bool RadixGraph::GetNeighboursByOffset(int src, std::vector<WeightedEdge> &neighbours, int timestamp) {
    auto& src_ptr = vertex_index->vertex_table[src];
    int num = 0, k = 0;
    int thread_id = omp_get_thread_num(), cnt = timestamp == -1 ? src_ptr.next.size() : timestamp, deg = src_ptr.deg;
    neighbours.resize(deg);
    for (int i = cnt - 1; i >= 0; i--) {
        auto e = src_ptr.next[i];
        if (!bitmap[thread_id]->get_bit(e.idx)) {
            if (e.weight != 0) { // Insert or Update
                // Have not found a previous log for this edge, thus this edge is the latest
                neighbours[num++] = e;
            }
            bitmap[thread_id]->set_bit(e.idx);
        }
        if (deg - num == i) {
            // Edge num = log num, all previous logs are materialized
            for (int j = i - 1; j >= 0; j--) {
                neighbours[num++] = src_ptr.next[j];
            }
            k = i;
            break;
        }
    }
    for (int i = k; i < cnt; i++) {
        bitmap[thread_id]->clear_bit(src_ptr.next[i].idx);
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

RadixGraph::RadixGraph(int d, std::vector<int> _num_children, bool _enable_query) {
    enable_query = _enable_query;
    if (enable_query) {
        degree = (std::atomic<int>*)calloc(CAP_DUMMY_NODES, sizeof(int));
        bitmap = new AtomicBitmap*[max_number_of_threads];
        for (int i = 0; i < max_number_of_threads; i++) bitmap[i] = new AtomicBitmap(CAP_DUMMY_NODES), bitmap[i]->reset();
    }
    vertex_index = new SORT(d, _num_children);
}

RadixGraph::~RadixGraph() {
    if (bitmap) {
        for (int i = 0; i < max_number_of_threads; i++) delete bitmap[i];
        delete [] bitmap;
    }
    if (degree) free(degree);
    delete vertex_index;
}