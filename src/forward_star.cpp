#include "forward_star.h"

bool ForwardStar::InsertEdge(uint64_t src, uint64_t des, double weight) {
    if (vertex_index.find(src) == vertex_index.end()) {
        auto tmp = new DummyNode{false, nullptr};
        vertex_index[src] = std::make_pair(tmp, nullptr);
        dummy_nodes.push_back(tmp);
    }
    if (vertex_index.find(des) == vertex_index.end()) {
        auto tmp = new DummyNode{false, nullptr};
        vertex_index[des] = std::make_pair(tmp, nullptr);
        dummy_nodes.push_back(tmp);
    }

    auto& tmp = vertex_index[src];
    if (tmp.second == nullptr) {
        tmp.first->next = new WeightedEdge{src, des, weight, vertex_index[des].first, nullptr};
        tmp.second = tmp.first->next;
    }
    else {
        tmp.second->next = new WeightedEdge{src, des, weight, vertex_index[des].first, nullptr};
        tmp.second = tmp.second->next;
    }
}

void ForwardStar::BFS(uint64_t src) {
    for (auto e : dummy_nodes) {
        e->obsolete = false;
    }
    std::queue<DummyNode*> Q;
    auto& tmp = vertex_index[src];
    tmp.first->obsolete = true;
    Q.push(tmp.first);
    while (!Q.empty()) {
        auto u = Q.front();
        Q.pop();
        auto e = u->next;
        while (e) {
            if (!e->forward->obsolete) {
                e->forward->obsolete = true;
                Q.push(e->forward);
            }
            e = e->next;
        }
    }
}

ForwardStar::~ForwardStar() {
    for (auto u : vertex_index) {
        auto e = u.second.first->next;
        while (e) {
            auto e_next = e->next;
            delete e;
            e = e_next;
        }
    }
    for (auto u : dummy_nodes) {
        delete u;
    }
}