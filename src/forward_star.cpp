#include "forward_star.h"

bool ForwardStar::InsertEdge(uint64_t src, uint64_t des, double weight) {
    if (vertex_index->RetrieveVertex(src) == nullptr) {
        auto tmp = new DummyNode{src, 0, std::vector<WeightedEdge*>()};
        vertex_index->InsertVertex(src, tmp);
        dummy_nodes.push_back(tmp);
    }
    if (vertex_index->RetrieveVertex(des) == nullptr) {
        auto tmp = new DummyNode{des, 0, std::vector<WeightedEdge*>()};
        vertex_index->InsertVertex(des, tmp);
        dummy_nodes.push_back(tmp);
    }

    auto tmp = (DummyNode*)vertex_index->RetrieveVertex(src);
    tmp->next.push_back(new WeightedEdge{weight, (DummyNode*)vertex_index->RetrieveVertex(des)});

    return true;
}

bool ForwardStar::UpdateEdge(uint64_t src, uint64_t des, double weight) {
    if (vertex_index->RetrieveVertex(src) == nullptr) {
        return false;
    }
    if (vertex_index->RetrieveVertex(des) == nullptr) {
        return false;
    }

    auto tmp = (DummyNode*)vertex_index->RetrieveVertex(src);
    tmp->next.push_back(new WeightedEdge{weight, (DummyNode*)vertex_index->RetrieveVertex(des)});

    return true;
}

bool ForwardStar::DeleteEdge(uint64_t src, uint64_t des) {
    if (vertex_index->RetrieveVertex(src) == nullptr) {
        return false;
    }
    if (vertex_index->RetrieveVertex(des) == nullptr) {
        return false;
    }

    auto tmp = (DummyNode*)vertex_index->RetrieveVertex(src);
    tmp->next.push_back(new WeightedEdge{0, (DummyNode*)vertex_index->RetrieveVertex(des)});

    return true;
}

bool ForwardStar::GetNeighbours(uint64_t src, std::vector<WeightedEdge*> &neighbours) {
    auto tmp = (DummyNode*)vertex_index->RetrieveVertex(src);
    if (tmp != nullptr) {
        for (int i = int(tmp->next.size()) - 1; i >= 0; i--) {
            auto e = tmp->next[i];
            e->forward->flag &= 1;
        }
        for (int i = int(tmp->next.size()) - 1; i >= 0; i--) {
            auto e = tmp->next[i];
            if (e->weight > 0 && (e->forward->flag & 2) == 0) {
                neighbours.push_back(e);
            }
            e->forward->flag |= 2;
        }
    }
    else {
        return false;
    }

    return true;
}

bool ForwardStar::GetNeighbours(DummyNode* src, std::vector<WeightedEdge*> &neighbours) {
    for (int i = int(src->next.size()) - 1; i >= 0; i--) {
        auto e = src->next[i];
        e->forward->flag &= 1;
    }
    for (int i = int(src->next.size()) - 1; i >= 0; i--) {
        auto e = src->next[i];
        if (e->weight > 0 && (e->forward->flag & 2) == 0) {
            neighbours.push_back(e);
        }
        e->forward->flag |= 2;
    }

    return true;
}

std::vector<uint64_t> ForwardStar::BFS(uint64_t src) {
    for (auto e : dummy_nodes) {
        e->flag &= 0;
    }
    std::queue<DummyNode*> Q;
    auto tmp = (DummyNode*)vertex_index->RetrieveVertex(src);
    tmp->flag |= 1;
    Q.push(tmp);
    std::vector<uint64_t> res;
    while (!Q.empty()) {
        auto u = Q.front();
        Q.pop();
        res.push_back(u->node);
        std::vector<WeightedEdge*> neighbours;
        GetNeighbours(u, neighbours);
        for (auto e : neighbours) {
            if ((e->forward->flag & 1) == 0) {
                e->forward->flag |= 1;
                Q.push(e->forward);
            }
        }
    }
    return res;
}

ForwardStar::ForwardStar(int d, std::vector<int> _num_children) {
    vertex_index = new Trie(d, _num_children);
}

ForwardStar::~ForwardStar() {
    for (auto u : dummy_nodes) {
        for (auto e : u->next) {
            delete e;
        }
        delete u;
    }
    delete vertex_index;
}