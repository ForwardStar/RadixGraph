#include "optimized_trie.h"

void Trie::InsertVertex(uint64_t id, DummyNode* node) {
    int num_bits = sum_children;
    TrieNode* current = root;
    while (true) {
        if (current->is_internal) {
            InternalNode* tmp = (InternalNode*)current;
            uint64_t idx = ((id & ((1ull << num_bits) - 1)) >> (num_bits - num_children[tmp->level]));
            if (tmp->children[idx] == nullptr) {
                if (tmp->level < depth - 1) {
                    tmp->children[idx] = new InternalNode{true, tmp->level + 1, std::vector<TrieNode*>(1 << num_children[tmp->level + 1])};
                    auto tmp_child = (InternalNode*)tmp->children[idx];
                    tmp_child->mtx.lock();
                }
                else {
                    tmp->children[idx] = new LeafNode{false, nullptr};
                    auto tmp_child = (LeafNode*)tmp->children[idx];
                    tmp_child->mtx.lock();
                }
                tmp->mtx.unlock();
            }
            current = tmp->children[idx];
            num_bits -= num_children[tmp->level];
        }
        else {
            LeafNode* tmp = (LeafNode*)current;
            tmp->head = node;
            tmp->mtx.unlock();
            break;
        }
    }
}

DummyNode* Trie::RetrieveVertex(uint64_t id, bool lock) {
    int num_bits = sum_children;
    TrieNode* current = root;
    while (current) {
        if (current->is_internal) {
            InternalNode* tmp = (InternalNode*)current;
            uint64_t idx = ((id & ((1ull << num_bits) - 1)) >> (num_bits - num_children[tmp->level]));
            if (lock && tmp->children[idx] == nullptr) {
                tmp->mtx.lock();
                if (tmp->children[idx]) {
                    tmp->mtx.unlock();
                }
            }
            current = tmp->children[idx];
            num_bits -= num_children[tmp->level];
        }
        else {
            LeafNode* tmp = (LeafNode*)current;
            if (lock && tmp->head == nullptr) {
                tmp->mtx.lock();
                if (tmp->head) {
                    tmp->mtx.unlock();
                }
            }
            return tmp->head;
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
        if (u->is_internal) {
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

Trie::Trie(int d, int _num_children[]) {
    depth = d;
    for (int i = 0; i < d; i++) {
        num_children.push_back(_num_children[i]);
        sum_children += num_children[i];
    }
    root = new InternalNode{true, 0, std::vector<TrieNode*>()};
}

Trie::Trie(int d, std::vector<int> _num_children) {
    depth = d;
    for (int i = 0; i < d; i++) {
        num_children.push_back(_num_children[i]);
        sum_children += num_children[i];
    }
    root = new InternalNode{true, 0, std::vector<TrieNode*>(1 << num_children[0])};
}

Trie::~Trie() {
    std::queue<TrieNode*> Q;
    Q.push(root);
    while (!Q.empty()) {
        TrieNode* u = Q.front();
        Q.pop();
        if (u->is_internal) {
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