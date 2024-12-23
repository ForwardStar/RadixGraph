#include "forward_star.h"

bool ForwardStar::InsertEdge(uint64_t src, uint64_t des, double weight) {
    DummyNode* src_ptr = vertex_index->RetrieveVertex(src, true);
    DummyNode* des_ptr = vertex_index->RetrieveVertex(des, true);
    src_ptr->next.emplace_back(WeightedEdge{weight, des_ptr, global_timestamp++, 1});
    return true;
}

bool ForwardStar::UpdateEdge(uint64_t src, uint64_t des, double weight) {
    DummyNode* src_ptr = vertex_index->RetrieveVertex(src);
    if (!src_ptr) {
        return false;
    }
    DummyNode* des_ptr = vertex_index->RetrieveVertex(des);
    if (!des_ptr) {
        return false;
    }
    src_ptr->next.emplace_back(WeightedEdge{weight, des_ptr, global_timestamp++, 2});
    return true;
}

bool ForwardStar::DeleteEdge(uint64_t src, uint64_t des) {
    DummyNode* src_ptr = vertex_index->RetrieveVertex(src);
    if (!src_ptr) {
        return false;
    }
    DummyNode* des_ptr = vertex_index->RetrieveVertex(des);
    if (!des_ptr) {
        return false;
    }
    src_ptr->next.emplace_back(WeightedEdge{0, des_ptr, global_timestamp++, 2});
    return true;
}

bool ForwardStar::GetNeighbours(uint64_t src, std::vector<WeightedEdge> &neighbours) {
    DummyNode* src_ptr = vertex_index->RetrieveVertex(src);
    if (!src_ptr) {
        return false;
    }
    return GetNeighbours(src_ptr, neighbours);
}

bool ForwardStar::GetNeighbours(DummyNode* src, std::vector<WeightedEdge> &neighbours) {
    std::vector<int> temp;
    if (src) {
        int thread_id = thread_pool[cnt.fetch_sub(1, std::memory_order_relaxed) - 1];
        for (int i = int(src->next.size()) - 1; i >= 0; i--) {
            src->next[i].forward->flag[thread_id] &= (1 << 7);
        }
        for (int i = int(src->next.size()) - 1; i >= 0; i--) {
            auto e = src->next[i];
            if ((e.forward->flag[thread_id] & 7) == 0) {
                if (e.flag == 1) {
                    neighbours.emplace_back(e);
                }
                else if (e.flag == 2) {
                    temp.emplace_back(i);
                }
            }
            if ((e.forward->flag[thread_id] & 5) == 0) {
                e.forward->flag[thread_id] |= e.flag;
            }
        }
        for (auto i : temp) {
            if (src->next[i].forward->flag[thread_id] & 1) {
                neighbours.emplace_back(src->next[i]);
            }
        }
        thread_pool[cnt.fetch_add(1, std::memory_order_relaxed)] = thread_id;
    }
    else {
        return false;
    }

    return true;
}

std::vector<DummyNode*> ForwardStar::BFS(uint64_t src) {
    std::queue<DummyNode*> Q;
    auto src_ptr = vertex_index->RetrieveVertex(src);
    src_ptr->flag[0] |= (1 << 7);
    Q.push(src_ptr);
    std::vector<DummyNode*> res;
    while (!Q.empty()) {
        auto u = Q.front();
        Q.pop();
        res.push_back(u);
        std::vector<WeightedEdge> neighbours;
        GetNeighbours(u, neighbours);
        for (auto e : neighbours) {
            if ((e.forward->flag[0] & (1 << 7)) == 0) {
                e.forward->flag[0] |= (1 << 7);
                Q.push(e.forward);
            }
        }
    }
    for (auto u : res) u->flag[0] = 0;
    return res;
}

ForwardStar::ForwardStar(int d, std::vector<int> _num_children) {
    vertex_index = new Trie(d, _num_children);
    for (int i = 0; i < max_number_of_threads; i++) {
        thread_pool.emplace_back(i), ++cnt;
    }
}

ForwardStar::~ForwardStar() {
    delete vertex_index;
}