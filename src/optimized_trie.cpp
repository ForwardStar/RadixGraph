#include "optimized_trie.h"

void Trie::InsertVertex(TrieNode* current, DummyNode* node) {
    uint64_t id = node->node;
    while (true) {
        if (current->level < depth) {
            InternalNode* tmp = (InternalNode*)current;
            int num_now = sum_bits[depth - 1] - (tmp->level > 0 ? sum_bits[tmp->level - 1] : 0);
            uint64_t idx = ((id & ((1ull << num_now) - 1)) >> (sum_bits[depth - 1] - sum_bits[tmp->level]));
            if (tmp->children[idx] == nullptr) {
                if (tmp->level < depth - 1) {
                    auto tmp_child = new InternalNode{tmp->level + 1, std::vector<TrieNode*>(1 << num_bits[tmp->level + 1])};
                    tmp_child->mtx.lock();
                    tmp->children[idx] = tmp_child;
                }
                else {
                    auto tmp_child = new LeafNode{tmp->level + 1, nullptr};
                    tmp_child->mtx.lock();
                    tmp->children[idx] = tmp_child;
                }
                tmp->mtx.unlock();
            }
            current = tmp->children[idx];
        }
        else {
            LeafNode* tmp = (LeafNode*)current;
            tmp->head = node;
            tmp->mtx.unlock();
            break;
        }
    }
}

void Trie::InsertVertex(DummyNode* node) {
    InsertVertex(root, node);
}

Trie::TrieNode* Trie::RetrieveVertex(uint64_t id, bool lock) {
    TrieNode* current = root;
    while (current) {
        if (current->level < depth) {
            InternalNode* tmp = (InternalNode*)current;
            int num_now = sum_bits[depth - 1] - (tmp->level > 0 ? sum_bits[tmp->level - 1] : 0);
            uint64_t idx = ((id & ((1ull << num_now) - 1)) >> (sum_bits[depth - 1] - sum_bits[tmp->level]));
            if (lock && tmp->children[idx] == nullptr) {
                tmp->mtx.lock();
                if (tmp->children[idx]) {
                    tmp->mtx.unlock();
                }
                else {
                    return tmp;
                }
            }
            current = tmp->children[idx];
        }
        else {
            LeafNode* tmp = (LeafNode*)current;
            if (lock && tmp->head == nullptr) {
                tmp->mtx.lock();
                if (tmp->head) {
                    tmp->mtx.unlock();
                }
            }
            return tmp;
        }
    }
    return nullptr;
}

long long Trie::size() {
    long long sz = 0;
    std::queue<TrieNode*> Q;
    Q.push(root);
    while (!Q.empty()) {
        TrieNode* u = Q.front();
        Q.pop();
        if (u->level < depth) {
            InternalNode* tmp = (InternalNode*)u;
            sz += tmp->children.size();
            for (auto nxt : tmp->children) {
                if (nxt) {
                    Q.push(nxt);
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
    root = new InternalNode{0, std::vector<TrieNode*>(1 << num_bits[0])};
}

Trie::Trie(int d, std::vector<int> _num_bits) {
    depth = d;
    num_bits.resize(d), sum_bits.resize(d);
    for (int i = 0; i < d; i++) {
        num_bits[i] = _num_bits[i];
        sum_bits[i] = (i > 0 ? sum_bits[i - 1] : 0) + num_bits[i];
    }
    root = new InternalNode{0, std::vector<TrieNode*>(1 << num_bits[0])};
}

Trie::~Trie() {
    std::queue<TrieNode*> Q;
    Q.push(root);
    while (!Q.empty()) {
        TrieNode* u = Q.front();
        Q.pop();
        if (u->level < depth) {
            InternalNode* tmp = (InternalNode*)u;
            for (auto nxt : tmp->children) {
                if (nxt) {
                    Q.push(nxt);
                }
            }
        }
        delete u;
    }
}