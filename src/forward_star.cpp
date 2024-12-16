#include "forward_star.h"

void ForwardStar::ExpandDummies() {
    dummy_nodes.emplace_back(new DummyNode[10000]);
    mtx.unlock();
}

bool ForwardStar::InsertEdge(uint64_t src, uint64_t des, double weight) {
    DummyNode* src_ptr = 0;
    auto tmp = vertex_index->RetrieveVertex(src, true);
    if (tmp->level == vertex_index->depth) {
        auto tmp_leaf = (Trie::LeafNode*)tmp;
        src_ptr = (DummyNode*)tmp_leaf->head;
    }
    if (!src_ptr) {
        int idx = num_dummy_nodes.fetch_add(1, std::memory_order_relaxed);
        int a = idx / 10000, b = idx % 10000;
        if (dummy_nodes.size() <= a) {
            mtx.lock();
            if (dummy_nodes.size() > a) {
                mtx.unlock();
            }
            else {
                ExpandDummies();
            }
        }
        dummy_nodes[a][b].node = src;
        src_ptr = &dummy_nodes[a][b];
        vertex_index->InsertVertex(tmp, src_ptr);
    }
    
    DummyNode* des_ptr = 0;
    tmp = vertex_index->RetrieveVertex(des, true);
    if (tmp->level == vertex_index->depth) {
        auto tmp_leaf = (Trie::LeafNode*)tmp;
        des_ptr = (DummyNode*)tmp_leaf->head;
    }
    if (!des_ptr) {
        int idx = num_dummy_nodes.fetch_add(1, std::memory_order_relaxed);
        int a = idx / 10000, b = idx % 10000;
        if (dummy_nodes.size() <= a) {
            mtx.lock();
            if (dummy_nodes.size() > a) {
                mtx.unlock();
            }
            else {
                ExpandDummies();
            }
        }
        dummy_nodes[a][b].node = des;
        des_ptr = &dummy_nodes[a][b];
        vertex_index->InsertVertex(tmp, des_ptr);
    }

    src_ptr->next.emplace_back(WeightedEdge{weight, des_ptr, 1});

    return true;
}

bool ForwardStar::UpdateEdge(uint64_t src, uint64_t des, double weight) {
    DummyNode* src_ptr = 0;
    auto tmp = vertex_index->RetrieveVertex(src, true);
    if (tmp->level == vertex_index->depth) {
        auto tmp_leaf = (Trie::LeafNode*)tmp;
        src_ptr = (DummyNode*)tmp_leaf->head;
    }
    if (!src_ptr) {
        return false;
    }

    DummyNode* des_ptr = 0;
    tmp = vertex_index->RetrieveVertex(des, true);
    if (tmp->level == vertex_index->depth) {
        auto tmp_leaf = (Trie::LeafNode*)tmp;
        des_ptr = (DummyNode*)tmp_leaf->head;
    }
    if (!des_ptr) {
        return false;
    }

    src_ptr->next.emplace_back(WeightedEdge{weight, des_ptr, 2});

    return true;
}

bool ForwardStar::DeleteEdge(uint64_t src, uint64_t des) {
    DummyNode* src_ptr = 0;
    auto tmp = vertex_index->RetrieveVertex(src, true);
    if (tmp->level == vertex_index->depth) {
        auto tmp_leaf = (Trie::LeafNode*)tmp;
        src_ptr = (DummyNode*)tmp_leaf->head;
    }
    if (!src_ptr) {
        return false;
    }

    DummyNode* des_ptr = 0;
    tmp = vertex_index->RetrieveVertex(des, true);
    if (tmp->level == vertex_index->depth) {
        auto tmp_leaf = (Trie::LeafNode*)tmp;
        des_ptr = (DummyNode*)tmp_leaf->head;
    }
    if (!des_ptr) {
        return false;
    }

    src_ptr->next.emplace_back(WeightedEdge{0, des_ptr, 4});

    return true;
}

bool ForwardStar::GetNeighbours(uint64_t src, std::vector<WeightedEdge> &neighbours) {
    DummyNode* src_ptr = 0;
    auto tmp = vertex_index->RetrieveVertex(src, true);
    if (tmp->level == vertex_index->depth) {
        auto tmp_leaf = (Trie::LeafNode*)tmp;
        src_ptr = (DummyNode*)tmp_leaf->head;
    }
    if (!src_ptr) {
        return false;
    }
    return GetNeighbours(src_ptr, neighbours);
}

bool ForwardStar::GetNeighbours(DummyNode* src, std::vector<WeightedEdge> &neighbours) {
    std::vector<WeightedEdge> temp;
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
                    temp.emplace_back(e);
                }
            }
            if ((e.forward->flag[thread_id] & 5) == 0) {
                e.forward->flag[thread_id] |= e.flag;
            }
        }
        for (auto e : temp) {
            if (e.forward->flag[thread_id] & 1) {
                neighbours.emplace_back(e);
            }
        }
        thread_pool[cnt.fetch_add(1, std::memory_order_relaxed)] = thread_id;
    }
    else {
        return false;
    }

    return true;
}

std::vector<uint64_t> ForwardStar::BFS(uint64_t src) {
    for (int i = 0; i < num_dummy_nodes; i++) {
        dummy_nodes[i / 10000][i % 10000].flag[0] &= 0;
    }
    std::queue<DummyNode*> Q;
    
    auto tmp = (Trie::LeafNode*)vertex_index->RetrieveVertex(src, true);
    auto src_ptr = (DummyNode*)tmp->head;
    src_ptr->flag[0] |= (1 << 7);
    Q.push(src_ptr);
    std::vector<uint64_t> res;
    while (!Q.empty()) {
        auto u = Q.front();
        Q.pop();
        res.push_back(u->node);
        std::vector<WeightedEdge> neighbours;
        GetNeighbours(u, neighbours);
        for (auto e : neighbours) {
            if ((e.forward->flag[0] & (1 << 7)) == 0) {
                e.forward->flag[0] |= (1 << 7);
                Q.push(e.forward);
            }
        }
    }
    return res;
}

ForwardStar::ForwardStar(int d, std::vector<int> _num_children) {
    vertex_index = new Trie(d, _num_children);
    for (int i = 0; i < 32; i++) {
        thread_pool.emplace_back(i), ++cnt;
    }
}

ForwardStar::~ForwardStar() {
    for (auto u : dummy_nodes) {
        delete [] u;
    }
    delete vertex_index;
}