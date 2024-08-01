#include "forward_star.h"

bool ForwardStar::InsertEdge(uint64_t src, uint64_t des, double weight) {
    if (vertex_index.find(src) == vertex_index.end()) {
        auto tmp = new WeightedEdge{0, 0, 0, false, nullptr, nullptr};
        vertex_index[src] = std::make_pair(tmp, tmp);
        head_edges.push_back(tmp);
    }
    if (vertex_index.find(des) == vertex_index.end()) {
        auto tmp = new WeightedEdge{0, 0, 0, false, nullptr, nullptr};
        vertex_index[des] = std::make_pair(tmp, tmp);
        head_edges.push_back(tmp);
    }

    auto& tmp = vertex_index[src];
    tmp.second->next = new WeightedEdge{src, des, weight, false, vertex_index[des].first, nullptr};
    tmp.second = tmp.second->next;
}

void ForwardStar::BFS(uint64_t src) {
    for (auto e : head_edges) {
        e->visited = false;
    }
    std::queue<WeightedEdge*> Q;
    auto& tmp = vertex_index[src];
    tmp.first->visited = true;
    Q.push(tmp.first);
    while (!Q.empty()) {
        auto u = Q.front();
        Q.pop();
        auto e = u->next;
        while (e) {
            if (!e->forward->visited) {
                e->forward->visited = true;
                Q.push(e->forward);
            }
            e = e->next;
        }
    }
}

ForwardStar::~ForwardStar() {
    for (auto u : vertex_index) {
        auto e = u.second.first;
        while (e) {
            auto e_next = e->next;
            delete e;
            e = e_next;
        }
    }
}