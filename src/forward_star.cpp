#include "forward_star.h"

bool ForwardStar::InsertEdge(uint64_t src, uint64_t des, double weight) {
    auto src_ptr = (DummyNode*)vertex_index->RetrieveVertex(src);
    if (src_ptr == nullptr) {
        int idx = num_dummy_nodes++ % 10000;
        if (idx == 0) {
            dummy_nodes.push_back(new DummyNode[10000]);
        }
        dummy_nodes.back()[idx].node = src;
        src_ptr = &dummy_nodes.back()[idx];
        vertex_index->InsertVertex(src, src_ptr);
    }
    
    auto des_ptr = (DummyNode*)vertex_index->RetrieveVertex(des);
    if (des_ptr == nullptr) {
        int idx = num_dummy_nodes++ % 10000;
        if (idx == 0) {
            dummy_nodes.push_back(new DummyNode[10000]);
        }
        dummy_nodes.back()[idx].node = des;
        des_ptr = &dummy_nodes.back()[idx];
        vertex_index->InsertVertex(des, des_ptr);
    }

    src_ptr->next.emplace_back(WeightedEdge{weight, des_ptr});

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
    tmp->next.emplace_back(WeightedEdge{weight, (DummyNode*)vertex_index->RetrieveVertex(des)});

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
    tmp->next.emplace_back(WeightedEdge{0, (DummyNode*)vertex_index->RetrieveVertex(des)});

    return true;
}

bool ForwardStar::GetNeighbours(uint64_t src, std::vector<WeightedEdge> &neighbours) {
    auto tmp = (DummyNode*)vertex_index->RetrieveVertex(src);
    if (tmp != nullptr) {
        for (int i = int(tmp->next.size()) - 1; i >= 0; i--) {
            tmp->next[i].forward->flag &= 1;
        }
        for (int i = int(tmp->next.size()) - 1; i >= 0; i--) {
            auto e = tmp->next[i];
            if (e.weight > 0 && (e.forward->flag & 2) == 0) {
                neighbours.emplace_back(e);
            }
            e.forward->flag |= 2;
        }
    }
    else {
        return false;
    }

    return true;
}

bool ForwardStar::GetNeighbours(DummyNode* src, std::vector<WeightedEdge> &neighbours) {
    for (int i = int(src->next.size()) - 1; i >= 0; i--) {
        auto e = src->next[i];
        e.forward->flag &= 1;
    }
    for (int i = int(src->next.size()) - 1; i >= 0; i--) {
        auto e = src->next[i];
        if (e.weight > 0 && (e.forward->flag & 2) == 0) {
            neighbours.emplace_back(e);
        }
        e.forward->flag |= 2;
    }

    return true;
}

std::vector<uint64_t> ForwardStar::BFS(uint64_t src) {
    for (int i = 0; i < num_dummy_nodes; i++) {
        dummy_nodes[i / 10000][i % 10000].flag &= 0;
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
        std::vector<WeightedEdge> neighbours;
        GetNeighbours(u, neighbours);
        for (auto e : neighbours) {
            if ((e.forward->flag & 1) == 0) {
                e.forward->flag |= 1;
                Q.push(e.forward);
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
        delete [] u;
    }
    delete vertex_index;
}