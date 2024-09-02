#include "adj_list.h"

bool AdjacencyLinkedList::InsertEdge(uint64_t src, uint64_t des, double weight) {
    if (vertex_index.find(src) == vertex_index.end()) {
        vertex_index[src] = nullptr;
    }
    if (vertex_index.find(des) == vertex_index.end()) {
        vertex_index[des] = nullptr;
    }

    auto& tmp = vertex_index[src];
    if (tmp) {
        tmp->next = new WeightedEdge{des, weight, tmp->next};
    }
    else {
        tmp = new WeightedEdge{des, weight, nullptr};
    }

    return true;
}

bool AdjacencyLinkedList::GetNeighbours(uint64_t src, std::vector<WeightedEdge*> &neighbours) {
    if (vertex_index.find(src) != vertex_index.end()) {
        auto e = vertex_index[src];
        while (e) {
            neighbours.push_back(e);
            e = e->next;
        }
    }

    return true;
}

void AdjacencyLinkedList::BFS(uint64_t src) {
    std::unordered_set<uint64_t> visited_vertices;
    std::queue<uint64_t> Q;
    visited_vertices.insert(src);
    Q.push(src);
    while (!Q.empty()) {
        auto u = Q.front();
        Q.pop();
        std::vector<WeightedEdge*> neighbours;
        GetNeighbours(u, neighbours);
        for (auto e : neighbours) {
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
        vertex_index[src] = std::vector<WeightedEdge*>();
    }
    if (vertex_index.find(des) == vertex_index.end()) {
        vertex_index[des] = std::vector<WeightedEdge*>();
    }

    vertex_index[src].push_back(new WeightedEdge{des, weight});

    return true;
}

bool AdjacencyArrayList::GetNeighbours(uint64_t src, std::vector<WeightedEdge*> &neighbours) {
    if (vertex_index.find(src) != vertex_index.end()) {
        for (auto e : vertex_index[src]) {
            neighbours.push_back(e);
        }
    }

    return true;
}

void AdjacencyArrayList::BFS(uint64_t src) {
    std::unordered_set<uint64_t> visited_vertices;
    std::queue<uint64_t> Q;
    visited_vertices.insert(src);
    Q.push(src);
    while (!Q.empty()) {
        auto u = Q.front();
        Q.pop();
        std::vector<WeightedEdge*> neighbours;
        GetNeighbours(u, neighbours);
        for (auto e : neighbours) {
            if (visited_vertices.find(e->des) == visited_vertices.end()) {
                visited_vertices.insert(e->des);
                Q.push(e->des);
            }
        }
    }
}

AdjacencyArrayList::~AdjacencyArrayList() {
    for (auto u : vertex_index) {
        for (auto e : u.second) {
            delete e;
        }
    }
}