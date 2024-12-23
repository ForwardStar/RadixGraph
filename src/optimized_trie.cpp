#include "optimized_trie.h"

DummyNode* Trie::InsertVertex(TrieNode* current, uint64_t id, int d) {
    for (int i = d; i < depth; i++) {
        int num_now = sum_bits[depth - 1] - (i > 0 ? sum_bits[i - 1] : 0);
        uint64_t idx = ((id & ((1ull << num_now) - 1)) >> (sum_bits[depth - 1] - sum_bits[i]));
        if (i < depth - 1) {
            if (!current->children) {
                uint8_t unlocked = 0;
                while (!current->mtx.compare_exchange_strong(unlocked, 1)) {
                    unlocked = 0;
                }
                if (!current->children) {
                    current->children = new TrieNode[1 << num_bits[i]];
                }
                current->mtx = 0;
            }
        }
        else {
            if (!current->head) {
                uint8_t unlocked = 0;
                while (!current->mtx.compare_exchange_strong(unlocked, 1)) {
                    unlocked = 0;
                }
                if (!current->head) {
                    current->head = new DummyNode[1 << num_bits[i]];
                }
                current->mtx = 0;
            }
            if (current->head[idx].node == -1) {
                current->head[idx].node = id;
                std::memset(current->head[idx].flag, 0, sizeof(current->head[idx].flag));
            }
            return &current->head[idx];
        }
        current = &current->children[idx];
    }
    return nullptr;
}

DummyNode* Trie::RetrieveVertex(uint64_t id, bool insert_mode) {
    TrieNode* current = &root;
    for (int i = 0; i < depth; i++) {
        int num_now = sum_bits[depth - 1] - (i > 0 ? sum_bits[i - 1] : 0);
        uint64_t idx = ((id & ((1ull << num_now) - 1)) >> (sum_bits[depth - 1] - sum_bits[i]));
        if (i < depth - 1) {
            if (insert_mode && !current->children) {
                return InsertVertex(current, id, i);
            }
        }
        else {
            if (insert_mode && (!current->head || current->head[idx].node == -1)) {
                return InsertVertex(current, id, i);
            }
            if (current->head[idx].node == -1) {
                return nullptr;
            }
            return &current->head[idx];
        }
        current = &current->children[idx];
    }
    return nullptr;
}

long long Trie::size() {
    long long sz = 0;
    std::queue<std::pair<TrieNode*, int>> Q;
    Q.emplace(&root, 0);
    while (!Q.empty()) {
        TrieNode* u = Q.front().first;
        int d = Q.front().second;
        Q.pop();
        if (d < depth) {
            sz += (1 << num_bits[d]);
            if (d < depth - 1) {
                for (int i = 0; i < (1 << num_bits[d]); i++) {
                    if (u->children[i].children || u->children[i].head) {
                        Q.emplace(&u->children[i], d + 1);
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
    root.children = new TrieNode[1 << num_bits[0]];
}

Trie::Trie(int d, std::vector<int> _num_bits) {
    depth = d;
    num_bits.resize(d), sum_bits.resize(d);
    for (int i = 0; i < d; i++) {
        num_bits[i] = _num_bits[i];
        sum_bits[i] = (i > 0 ? sum_bits[i - 1] : 0) + num_bits[i];
    }
    root.children = new TrieNode[1 << num_bits[0]];
}