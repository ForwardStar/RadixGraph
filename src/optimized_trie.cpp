#include "optimized_trie.h"

void Trie::InsertVertex(TrieNode* current, DummyNode* u_ptr) {
    uint64_t id = u_ptr->node;
    while (true) {
        int num_now = sum_bits[depth - 1] - (current->level > 0 ? sum_bits[current->level - 1] : 0);
        uint64_t idx = ((id & ((1ull << num_now) - 1)) >> (sum_bits[depth - 1] - sum_bits[current->level]));
        if (current->children[idx].level == -1) {
            if (current->level < depth - 1) {
                current->children[idx].children = new TrieNode[1 << num_bits[current->level + 1]];
                uint64_t nxt_idx = ((id & ((1ull << num_now - num_bits[current->level]) - 1)) >> (sum_bits[depth - 1] - sum_bits[current->level + 1]));
                current->children[idx].children[nxt_idx].mtx.lock();
                current->children[idx].level = current->level + 1;
                current->children[idx].mtx.unlock();
            }
            else {
                current->children[idx].level = current->level + 1;
                current->children[idx].head = u_ptr;
                current->children[idx].mtx.unlock();
                break;
            }
        }
        current = &current->children[idx];
    }
}

void Trie::InsertVertex(DummyNode* u_ptr) {
    InsertVertex(&root, u_ptr);
}

Trie::TrieNode* Trie::RetrieveVertex(uint64_t id, bool lock) {
    TrieNode* current = &root;
    while (current) {
        if (current->level < depth) {
            int num_now = sum_bits[depth - 1] - (current->level > 0 ? sum_bits[current->level - 1] : 0);
            uint64_t idx = ((id & ((1ull << num_now) - 1)) >> (sum_bits[depth - 1] - sum_bits[current->level]));
            if (lock && current->children[idx].level == -1) {
                current->children[idx].mtx.lock();
                if (current->children[idx].level == -1) {
                    return current;
                }
                current->children[idx].mtx.unlock();
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