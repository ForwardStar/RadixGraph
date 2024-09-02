#include "forward_star.h"

bool ChainedForwardStar::InsertEdge(uint64_t src, uint64_t des, double weight) {
    if (vertex_index.find(src) == vertex_index.end()) {
        auto tmp = new DummyNode{src, false, nullptr};
        vertex_index[src] = tmp;
        dummy_nodes.push_back(tmp);
    }
    if (vertex_index.find(des) == vertex_index.end()) {
        auto tmp = new DummyNode{des, false, nullptr};
        vertex_index[des] = tmp;
        dummy_nodes.push_back(tmp);
    }

    auto tmp = vertex_index[src];
    tmp->next = new WeightedEdge{weight, vertex_index[des], tmp->next};

    return true;
}

bool ChainedForwardStar::UpdateEdge(uint64_t src, uint64_t des, double weight) {
    if (vertex_index.find(src) == vertex_index.end()) {
        return false;
    }
    if (vertex_index.find(des) == vertex_index.end()) {
        return false;
    }

    auto tmp = vertex_index[src];
    tmp->next = new WeightedEdge{weight, vertex_index[des], tmp->next};

    return true;
}

bool ChainedForwardStar::DeleteEdge(uint64_t src, uint64_t des) {
    if (vertex_index.find(src) == vertex_index.end()) {
        return false;
    }
    if (vertex_index.find(des) == vertex_index.end()) {
        return false;
    }

    auto tmp = vertex_index[src];
    tmp->next = new WeightedEdge{0, vertex_index[des], tmp->next};

    return true;
}

bool ChainedForwardStar::GetNeighbours(uint64_t src, std::vector<WeightedEdge*> &neighbours) {
    if (vertex_index.find(src) != vertex_index.end()) {
        auto tmp = vertex_index[src];
        auto cur = tmp->next;
        while (cur) {
            cur->forward->flag &= 1;
            cur = cur->next;
        }
        cur = tmp->next;
        while (cur) {
            if (cur->weight > 0 && (cur->forward->flag & 2) == 0) {
                neighbours.push_back(cur);
            }
            cur->forward->flag |= 2;
            cur = cur->next;
        }
    }

    return true;
}

bool ChainedForwardStar::GetNeighbours(DummyNode* src, std::vector<WeightedEdge*> &neighbours) {
    auto cur = src->next;
    while (cur) {
        cur->forward->flag &= 1;
        cur = cur->next;
    }
    cur = src->next;
    while (cur) {
        if (cur->weight > 0 && (cur->forward->flag & 2) == 0) {
            neighbours.push_back(cur);
        }
        cur->forward->flag |= 2;
        cur = cur->next;
    }

    return true;
}

void ChainedForwardStar::BFS(uint64_t src) {
    for (auto e : dummy_nodes) {
        e->flag &= 0;
    }
    std::queue<DummyNode*> Q;
    auto& tmp = vertex_index[src];
    tmp->flag |= 1;
    Q.push(tmp);
    while (!Q.empty()) {
        auto u = Q.front();
        Q.pop();
        std::vector<WeightedEdge*> neighbours;
        GetNeighbours(u, neighbours);
        for (auto e : neighbours) {
            if ((e->forward->flag & 1) == 0) {
                e->forward->flag |= 1;
                Q.push(e->forward);
            }
        }
    }
}

ChainedForwardStar::~ChainedForwardStar() {
    for (auto u : vertex_index) {
        auto e = u.second->next;
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

bool ArrayForwardStar::InsertEdge(uint64_t src, uint64_t des, double weight) {
    if (vertex_index.find(src) == vertex_index.end()) {
        auto tmp = new DummyNode{src, false, std::vector<WeightedEdge*>()};
        vertex_index[src] = tmp;
        dummy_nodes.push_back(tmp);
    }
    if (vertex_index.find(des) == vertex_index.end()) {
        auto tmp = new DummyNode{des, false, std::vector<WeightedEdge*>()};
        vertex_index[des] = tmp;
        dummy_nodes.push_back(tmp);
    }

    vertex_index[src]->next.push_back(new WeightedEdge{weight, vertex_index[des]});

    return true;
}

bool ArrayForwardStar::UpdateEdge(uint64_t src, uint64_t des, double weight) {
    if (vertex_index.find(src) == vertex_index.end()) {
        return false;
    }
    if (vertex_index.find(des) == vertex_index.end()) {
        return false;
    }

    auto tmp = vertex_index[src];
    tmp->next.push_back(new WeightedEdge{weight, vertex_index[des]});

    return true;
}

bool ArrayForwardStar::DeleteEdge(uint64_t src, uint64_t des) {
    if (vertex_index.find(src) == vertex_index.end()) {
        return false;
    }
    if (vertex_index.find(des) == vertex_index.end()) {
        return false;
    }

    auto tmp = vertex_index[src];
    tmp->next.push_back(new WeightedEdge{0, vertex_index[des]});

    return true;
}

bool ArrayForwardStar::GetNeighbours(uint64_t src, std::vector<WeightedEdge*> &neighbours) {
    if (vertex_index.find(src) != vertex_index.end()) {
        auto tmp = vertex_index[src];
        for (int i = int(tmp->next.size() - 1); i >= 0; i--) {
            auto e = tmp->next[i];
            e->forward->flag &= 1;
        }
        for (int i = int(tmp->next.size() - 1); i >= 0; i--) {
            auto e = tmp->next[i];
            if (e->weight > 0 && (e->forward->flag & 2) == 0) {
                neighbours.push_back(e);
            }
            e->forward->flag |= 2;
        }
    }

    return true;
}

bool ArrayForwardStar::GetNeighbours(DummyNode* src, std::vector<WeightedEdge*> &neighbours) {
    for (int i = int(src->next.size() - 1); i >= 0; i--) {
        auto e = src->next[i];
        e->forward->flag &= 1;
    }
    for (int i = int(src->next.size() - 1); i >= 0; i--) {
        auto e = src->next[i];
        if (e->weight > 0 && (e->forward->flag & 2) == 0) {
            neighbours.push_back(e);
        }
        e->forward->flag |= 2;
    }

    return true;
}

void ArrayForwardStar::BFS(uint64_t src) {
    for (auto e : dummy_nodes) {
        e->flag &= 0;
    }
    std::queue<DummyNode*> Q;
    auto& tmp = vertex_index[src];
    tmp->flag |= 1;
    Q.push(tmp);
    while (!Q.empty()) {
        auto u = Q.front();
        Q.pop();
        std::vector<WeightedEdge*> neighbours;
        GetNeighbours(u, neighbours);
        for (auto e : neighbours) {
            if ((e->forward->flag & 1) == 0) {
                e->forward->flag |= 1;
                Q.push(e->forward);
            }
        }
    }
}

ArrayForwardStar::~ArrayForwardStar() {
    for (auto u : dummy_nodes) {
        for (auto e : u->next) {
            delete e;
        }
        delete u;
    }
}