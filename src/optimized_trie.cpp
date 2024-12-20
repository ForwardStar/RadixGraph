#include "optimized_trie.h"

Trie::TrieNode* Trie::InsertVertex(TrieNode* current, uint64_t u) {
    while (true) {
        int num_now = sum_bits[depth - 1] - (current->level > 0 ? sum_bits[current->level - 1] : 0);
        uint64_t idx = ((u & ((1ull << num_now) - 1)) >> (sum_bits[depth - 1] - sum_bits[current->level]));
        if (current->children[idx].level == -1) {
            if (current->level < depth - 1) {
                uint64_t nxt_idx = ((u & ((1ull << num_now - num_bits[current->level]) - 1)) >> (sum_bits[depth - 1] - sum_bits[current->level + 1]));
                current->children[idx].children = new TrieNode[1 << num_bits[current->level + 1]];
                current->children[idx].children[nxt_idx].mtx = 1;
                current->children[idx].level = current->level + 1;
                current->children[idx].mtx = 0;
            }
            else {
                return &current->children[idx];
            }
        }
        current = &current->children[idx];
    }
}

Trie::TrieNode* Trie::RetrieveVertex(uint64_t id, bool insert_mode) {
    TrieNode* current = &root;
    while (current) {
        if (current->level < depth) {
            int num_now = sum_bits[depth - 1] - (current->level > 0 ? sum_bits[current->level - 1] : 0);
            uint64_t idx = ((id & ((1ull << num_now) - 1)) >> (sum_bits[depth - 1] - sum_bits[current->level]));
            if (insert_mode && current->children[idx].level == -1) {
                int unlocked = 0;
                while (!current->children[idx].mtx.compare_exchange_strong(unlocked, 1)) {
                    unlocked = 0;
                }
                if (current->children[idx].level == -1) {
                    return InsertVertex(current, id);
                }
                current->children[idx].mtx = 0;
            }
            current = &current->children[idx];
        }
        else {
            return current;
        }
    }
    return nullptr;
}

long long Trie::size() {
    long long sz = 0;
    std::queue<TrieNode*> Q;
    Q.push(&root);
    while (!Q.empty()) {
        TrieNode* u = Q.front();
        Q.pop();
        if (u->level < depth) {
            sz += (1 << num_bits[u->level]);
            for (int i = 0; i < (1 << num_bits[u->level]); i++) {
                if (u->children[i].level != -1) {
                    Q.push(&u->children[i]);
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
    root.level = 0;
    root.children = new TrieNode[1 << num_bits[0]];
}

Trie::Trie(int d, std::vector<int> _num_bits) {
    depth = d;
    num_bits.resize(d), sum_bits.resize(d);
    for (int i = 0; i < d; i++) {
        num_bits[i] = _num_bits[i];
        sum_bits[i] = (i > 0 ? sum_bits[i - 1] : 0) + num_bits[i];
    }
    root.level = 0;
    root.children = new TrieNode[1 << num_bits[0]];
}

Trie::~Trie() {}