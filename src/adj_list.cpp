#include "adj_list.h"

bool AdjacencyLinkedList::InsertEdge(uint64_t src, uint64_t des, double weight) {
    if (vertex_index.find(src) == vertex_index.end()) {
        vertex_index[src] = nullptr;
    }
    if (vertex_index.find(des) == vertex_index.end()) {
        vertex_index[des] = nullptr;
    }

    vertex_index[src] = new WeightedEdge{des, weight, vertex_index[src]};
}

void AdjacencyLinkedList::BFS(uint64_t src) {
    std::unordered_set<uint64_t> visited_vertices;
    std::queue<uint64_t> Q;
    visited_vertices.insert(src);
    Q.push(src);
    while (!Q.empty()) {
        auto u = Q.front();
        Q.pop();
        auto e = vertex_index[u];
        while (e) {
            if (visited_vertices.find(e->des) == visited_vertices.end()) {
                visited_vertices.insert(e->des);
                Q.push(e->des);
            }
            e = e->next;
        }
    }
}

AdjacencyLinkedList::~AdjacencyLinkedList() {
    for (auto u : vertex_index) {
        auto e = u.second;
        while (e) {
            auto e_next = e->next;
            delete e;
            e = e_next;
        }
    }
}

bool AdjacencyArrayList::InsertEdge(uint64_t src, uint64_t des, double weight) {
    if (vertex_index.find(src) == vertex_index.end()) {
        vertex_index[src] = std::vector<WeightedEdge>();
    }
    if (vertex_index.find(des) == vertex_index.end()) {
        vertex_index[des] = std::vector<WeightedEdge>();
    }

    vertex_index[src].push_back(WeightedEdge{des, weight});
}

void AdjacencyArrayList::BFS(uint64_t src) {
    std::unordered_set<uint64_t> visited_vertices;
    std::queue<uint64_t> Q;
    visited_vertices.insert(src);
    Q.push(src);
    while (!Q.empty()) {
        auto u = Q.front();
        Q.pop();
        for (auto e : vertex_index[u]) {
            if (visited_vertices.find(e.des) == visited_vertices.end()) {
                visited_vertices.insert(e.des);
                Q.push(e.des);
            }
        }
    }
}