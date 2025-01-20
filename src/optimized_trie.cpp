#include "optimized_trie.h"

DummyNode* Trie::InsertVertex(TrieNode* current, uint64_t id, int d) {
    for (int i = d; i < depth; i++) {
        int num_now = sum_bits[depth - 1] - (i > 0 ? sum_bits[i - 1] : 0);
        uint64_t idx = ((id & ((1ull << num_now) - 1)) >> (sum_bits[depth - 1] - sum_bits[i]));
        if (i < depth - 1) {
            if (!current->children[idx]) {
                current->mtx->set_bit_atomic(idx);
                if (!current->children[idx]) {
                    auto tmp = new TrieNode();
                    tmp->sz = (1 << num_bits[i + 1]);
                    tmp->mtx = new AtomicBitmap(tmp->sz);
                    if (i + 1 < depth - 1) {
                        tmp->children = new TrieNode*[tmp->sz];
                        std::memset(tmp->children, 0, sizeof(tmp->children) * tmp->sz);
                    }
                    else {
                        tmp->head = new DummyNode*[tmp->sz];
                        std::memset(tmp->head, 0, sizeof(tmp->head) * tmp->sz);
                    }
                    current->children[idx] = tmp;
                }
                current->mtx->clear_bit(idx);
            }
        }
        else {
            if (!current->head[idx] || current->head[idx]->node == -1) {
                current->mtx->set_bit_atomic(idx);
                if (!current->head[idx]) {
                    int i = cnt.fetch_add(1);
                    current->head[idx] = new DummyNode();
                    current->head[idx]->idx = i;
                    if (i >= cap) {
                        uint8_t unlocked = 0;
                        while (!mtx.compare_exchange_strong(unlocked, 1)) {
                            unlocked = 0;
                        }
                        if (i >= cap) {
                            auto des = new DummyNode*[cap * 2];
                            std::memset(des, 0, sizeof(des) * cap * 2);
                            std::copy(dummy_nodes, dummy_nodes + cap, des);
                            delete [] dummy_nodes;
                            dummy_nodes = des;
                            cap *= 2;
                        }
                        mtx = 0;
                    }
                    dummy_nodes[i] = current->head[idx];
                }
                if (current->head[idx]->node == -1) {
                    current->head[idx]->flag = new AtomicBitmap(bitmap_size);
                    current->head[idx]->flag->reset();
                    current->head[idx]->next = new WeightedEdge[5];
                    current->head[idx]->cap = 5;
                    current->head[idx]->node = id;
                }
                current->mtx->clear_bit(idx);
            }
            return current->head[idx];
        }
        current = current->children[idx];
    }
    return nullptr;
}

DummyNode* Trie::RetrieveVertex(uint64_t id, bool insert_mode) {
    TrieNode* current = root;
    for (int i = 0; i < depth; i++) {
        int num_now = sum_bits[depth - 1] - (i > 0 ? sum_bits[i - 1] : 0);
        uint64_t idx = ((id & ((1ull << num_now) - 1)) >> (sum_bits[depth - 1] - sum_bits[i]));
        if (i < depth - 1) {
            if (!current->children[idx]) {
                if (insert_mode) {
                    return InsertVertex(current, id, i);
                }
                else {
                    return nullptr;
                }
            }
        }
        else {
            if (insert_mode && (!current->head[idx] || current->head[idx]->node == -1)) {
                return InsertVertex(current, id, i);
            }
            if (!current->head[idx] || current->head[idx]->node == -1) {
                return nullptr;
            }
            return current->head[idx];
        }
        current = current->children[idx];
    }
    return nullptr;
}

bool Trie::DeleteVertex(uint64_t id) {
    DummyNode* tmp = RetrieveVertex(id);
    if (tmp == nullptr) {
        return false;
    }
    tmp->node = -1;
    delete [] tmp->next;
    delete tmp->flag;
    tmp->cap = tmp->cnt = 0;
    return true;
}

long long Trie::size() {
    long long sz = 0;
    std::queue<std::pair<TrieNode*, int>> Q;
    Q.emplace(root, 0);
    while (!Q.empty()) {
        TrieNode* u = Q.front().first;
        int d = Q.front().second;
        Q.pop();
        if (d < depth) {
            sz += (1 << num_bits[d]);
            if (d < depth - 1) {
                for (int i = 0; i < (1 << num_bits[d]); i++) {
                    if (u->children[i]) {
                        Q.emplace(u->children[i], d + 1);
                    }
                }
            }
        }
    }
    return sz;
}

Trie::Trie(int d, int _num_bits[]) {
    depth = d;
    num_bits.resize(d), sum_bits.resize(d);
    for (int i = 0; i < d; i++) {
        num_bits[i] = _num_bits[i];
        sum_bits[i] = (i > 0 ? sum_bits[i - 1] : 0) + num_bits[i];
    }
    root = new TrieNode();
    root->sz = (1 << num_bits[0]);
    root->children = new TrieNode*[root->sz];
    std::memset(root->children, 0, sizeof(root->children) * root->sz);
    root->mtx = new AtomicBitmap(root->sz);
    cap = 1000;
    dummy_nodes = new DummyNode*[cap];
}

Trie::Trie(int d, std::vector<int> _num_bits) {
    depth = d;
    num_bits.resize(d), sum_bits.resize(d);
    for (int i = 0; i < d; i++) {
        num_bits[i] = _num_bits[i];
        sum_bits[i] = (i > 0 ? sum_bits[i - 1] : 0) + num_bits[i];
    }
    root = new TrieNode();
    root->sz = (1 << num_bits[0]);
    root->children = new TrieNode*[root->sz];
    std::memset(root->children, 0, sizeof(root->children) * root->sz);
    root->mtx = new AtomicBitmap(root->sz);
    cap = 1000;
    dummy_nodes = new DummyNode*[cap];
}

Trie::~Trie() {
    delete root;
    delete [] dummy_nodes;
}