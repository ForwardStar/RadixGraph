#include "adj_list.h"

bool AdjacencyList::InsertEdge(uint64_t src, uint64_t des, double weight) {
    if (vertex_index.find(src) == vertex_index.end()) {
        vertex_index[src] = nullptr;
    }
    if (vertex_index.find(des) == vertex_index.end()) {
        vertex_index[des] = nullptr;
    }

    vertex_index[src] = new WeightedEdge{src, des, false, weight, vertex_index[src]};
}

void AdjacencyList::BFS(uint64_t src) {
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