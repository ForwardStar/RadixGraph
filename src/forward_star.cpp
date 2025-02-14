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
#include "forward_star.h"

bool ForwardStar::Insert(DummyNode* src, DummyNode* des, double weight) {
    int i = src->cnt.fetch_add(1);
    if (i >= src->cap) {
        while (src->mtx.test_and_set()) {}
        if (i >= src->cap) {
            auto des = new WeightedEdge[src->cap * 2];
            std::copy(src->next, src->next + src->cap, des);
            delete [] src->next;
            src->next = des;
            src->cap *= 2;
        }
        src->mtx.clear();
    }
    src->next[i].forward = des;
    src->next[i].weight = weight;
    return true;
}

bool ForwardStar::InsertEdge(NodeID src, NodeID des, double weight) {
    DummyNode* src_ptr = vertex_index->RetrieveVertex(src, true);
    DummyNode* des_ptr = vertex_index->RetrieveVertex(des, true);
    src_ptr->deg.fetch_add(1);
    Insert(src_ptr, des_ptr, weight);
    return true;
}

bool ForwardStar::UpdateEdge(NodeID src, NodeID des, double weight) {
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

bool ForwardStar::DeleteEdge(NodeID src, NodeID des) {
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

bool ForwardStar::GetNeighbours(NodeID src, std::vector<WeightedEdge> &neighbours, int timestamp) {
    DummyNode* src_ptr = vertex_index->RetrieveVertex(src);
    if (!src_ptr) {
        return false;
    }
    return GetNeighbours(src_ptr, neighbours, timestamp);
}

bool ForwardStar::GetNeighbours(DummyNode* src, std::vector<WeightedEdge> &neighbours, int timestamp) {
    if (src) {
        int num = 0;
        int thread_id = omp_get_thread_num(), cnt = timestamp == -1 ? int(src->cnt) : timestamp, deg = src->deg;
        neighbours.resize(deg);
        for (int i = cnt - 1; i >= 0; i--) {
            if (src->next[i].forward->node == -1) {
                continue;
            }
            src->next[i].forward->flag->clear_bit(thread_id);
        }
        for (int i = cnt - 1; i >= 0; i--) {
            auto e = &src->next[i];
            if (e->forward->node == -1) {
                continue;
            }
            if (!e->forward->flag->get_bit(thread_id)) {
                if (e->weight != 0) { // Insert or Update
                    // Have not found a previous log for this edge, thus this edge is the latest
                    neighbours[num].forward = e->forward;
                    neighbours[num].weight = e->weight;
                    ++num;
                }
                e->forward->flag->set_bit(thread_id);
            }
        }
    }
    else {
        return false;
    }

    return true;
}

std::vector<DummyNode*> ForwardStar::BFS(NodeID src) {
    std::queue<DummyNode*> Q;
    AtomicBitmap vis(vertex_index->cnt);
    vis.reset();
    auto src_ptr = vertex_index->RetrieveVertex(src);
    vis.set_bit(src_ptr->idx);
    Q.push(src_ptr);
    std::vector<DummyNode*> res;
    while (!Q.empty()) {
        auto u = Q.front();
        Q.pop();
        res.push_back(u);
        std::vector<WeightedEdge> neighbours;
        GetNeighbours(u, neighbours);
        for (auto e : neighbours) {
            if (!vis.get_bit(e.forward->idx)) {
                vis.set_bit(e.forward->idx);
                Q.push(e.forward);
            }
        }
    }
    return res;
}

std::vector<double> ForwardStar::SSSP(NodeID src) {
    std::vector<double> dist;
    dist.assign(vertex_index->cnt, 1e9);
    auto u = vertex_index->RetrieveVertex(src);
    std::priority_queue<std::pair<double, DummyNode*>> Q;
    dist[u->idx] = 0;
    Q.emplace(0, u);
    while (!Q.empty()) {
        auto v = Q.top().second;
        Q.pop();
        std::vector<WeightedEdge> neighbours;
        GetNeighbours(v, neighbours);
        for (auto e : neighbours) {
            auto w = e.forward;
            if (dist[v->idx] + e.weight < dist[w->idx]) {
                dist[w->idx] = dist[v->idx] + e.weight;
                Q.emplace(-dist[w->idx], w);
            }
        }
    }
    return dist;
}

ForwardStar::ForwardStar(int d, std::vector<int> _num_children, bool enable_query) {
    vertex_index = new Trie(d, _num_children, enable_query);
}

ForwardStar::~ForwardStar() {
    delete vertex_index;
}