#include "forward_star.h"

bool ForwardStar::Insert(DummyNode* src, DummyNode* des, double weight, int type) {
    int i = src->cnt.fetch_add(1);
    if (i >= src->cap) {
        uint8_t unlocked = 0;
        while (!src->mtx.compare_exchange_strong(unlocked, 1)) {
            unlocked = 0;
        }
        if (i >= src->cap) {
            auto des = new WeightedEdge[src->cap * 2];
            std::copy(src->next, src->next + src->cap, des);
            delete [] src->next;
            src->next = des;
            src->cap *= 2;
        }
        src->mtx = 0;
    }
    if (i < src->cap) {
        src->next[i].forward = des;
        src->next[i].weight = weight;
        src->next[i].type = type;
        return true;
    }
    return false;
}

bool ForwardStar::InsertEdge(uint64_t src, uint64_t des, double weight) {
    DummyNode* src_ptr = vertex_index->RetrieveVertex(src, true);
    DummyNode* des_ptr = vertex_index->RetrieveVertex(des, true);
    Insert(src_ptr, des_ptr, weight, 1);
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
    Insert(src_ptr, des_ptr, weight, 2);
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
    Insert(src_ptr, des_ptr, 0, 3);
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
    if (src) {
        std::vector<int> temp;
        int num = 0;
        neighbours.resize(src->cnt);
        int thread_id = thread_pool[cnt.fetch_sub(1, std::memory_order_relaxed) - 1];
        const int INSERT = thread_id * 3, UPDATE = thread_id * 3 + 1, DELETE = thread_id * 3 + 2;
        for (int i = src->cnt - 1; i >= 0; i--) {
            if (src->next[i].forward->node == -1) {
                continue;
            }
            src->next[i].forward->flag->clear_bit(INSERT);
            src->next[i].forward->flag->clear_bit(UPDATE);
            src->next[i].forward->flag->clear_bit(DELETE);
        }
        for (int i = src->cnt - 1; i >= 0; i--) {
            auto e = src->next[i];
            if (e.forward->node == -1) {
                continue;
            }
            if (!e.forward->flag->get_bit(INSERT) && !e.forward->flag->get_bit(DELETE)) {
                if (!e.forward->flag->get_bit(UPDATE)) {
                    if (e.type == 1) { // Insert
                        neighbours[num++] = e;
                    }
                    else if (e.type == 2) { // Update
                        temp.emplace_back(i);
                    }
                }
                if (e.type == 1) {
                    e.forward->flag->set_bit(INSERT);
                }
                if (e.type == 2) {
                    e.forward->flag->set_bit(UPDATE);
                }
                if (e.type == 3) {
                    e.forward->flag->set_bit(DELETE);
                }
            }
        }
        for (auto i : temp) {
            if (src->next[i].forward->flag->get_bit(INSERT)) {
                neighbours[num++] = src->next[i];
            }
        }
        thread_pool[cnt.fetch_add(1, std::memory_order_relaxed)] = thread_id;
        while (neighbours.size() > num) {
            neighbours.pop_back();
        }
    }
    else {
        return false;
    }

    return true;
}

std::vector<DummyNode*> ForwardStar::BFS(uint64_t src) {
    std::queue<DummyNode*> Q;
    Bitmap vis(vertex_index->cnt);
    vis.reset();
    auto src_ptr = vertex_index->RetrieveVertex(src);
    vis.set_bit(src_ptr->idx);
    Q.push(src_ptr);
    std::vector<DummyNode*> res;
    while (!Q.empty()) {
        auto u = Q.front();
        Q.pop();
        res.push_back(u);
        std::vector<WeightedEdge> neighbours;
        GetNeighbours(u, neighbours);
        for (auto e : neighbours) {
            if (!vis.get_bit(e.forward->idx)) {
                vis.set_bit(e.forward->idx);
                Q.push(e.forward);
            }
        }
    }
    return res;
}

std::vector<double> ForwardStar::SSSP(uint64_t src) {
    std::vector<double> dist;
    dist.assign(vertex_index->cnt, 1e9);
    auto u = vertex_index->RetrieveVertex(src);
    std::priority_queue<std::pair<double, DummyNode*>> Q;
    dist[u->idx] = 0;
    Q.emplace(0, u);
    while (!Q.empty()) {
        auto v = Q.top().second;
        Q.pop();
        std::vector<WeightedEdge> neighbours;
        GetNeighbours(v, neighbours);
        for (auto e : neighbours) {
            auto w = e.forward;
            if (dist[v->idx] + e.weight < dist[w->idx]) {
                dist[w->idx] = dist[v->idx] + e.weight;
                Q.emplace(-dist[w->idx], w);
            }
        }
    }
    return dist;
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