#include "optimized_trie.h"

void Trie::InsertVertex(uint64_t id, DummyNode* node, int level) {
    int num_bits = sum_children;
    TrieNode* current = root;
    while (true) {
        if (current->is_internal) {
            InternalNode* tmp = (InternalNode*)current;
            uint64_t idx = ((id & ((1ull << num_bits) - 1)) >> (num_bits - num_children[level]));
            if (tmp->children[idx] == nullptr) {
                if (level < depth - 1) {
                    auto tmp_child = new InternalNode{true, std::vector<TrieNode*>(1 << num_children[level + 1])};
                    tmp_child->mtx.lock();
                    tmp->children[idx] = tmp_child;
                }
                else {
                    auto tmp_child = new LeafNode{false, nullptr};
                    tmp_child->mtx.lock();
                    tmp->children[idx] = tmp_child;
                }
                tmp->mtx.unlock();
            }
            current = tmp->children[idx];
            num_bits -= num_children[level];
            level++;
        }
        else {
            LeafNode* tmp = (LeafNode*)current;
            tmp->head = node;
            tmp->mtx.unlock();
            break;
        }
    }
}

DummyNode* Trie::RetrieveVertex(uint64_t id, bool lock, int level) {
    int num_bits = sum_children;
    TrieNode* current = root;
    while (current) {
        if (current->is_internal) {
            InternalNode* tmp = (InternalNode*)current;
            uint64_t idx = ((id & ((1ull << num_bits) - 1)) >> (num_bits - num_children[level]));
            if (lock && tmp->children[idx] == nullptr) {
                tmp->mtx.lock();
                if (tmp->children[idx]) {
                    tmp->mtx.unlock();
                }
            }
            current = tmp->children[idx];
            num_bits -= num_children[level];
            level++;
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
    num_children.resize(d);
    for (int i = 0; i < d; i++) {
        num_children[i] = _num_children[i];
        sum_children += num_children[i];
    }
    root = new InternalNode{true, std::vector<TrieNode*>()};
}

Trie::Trie(int d, std::vector<int> _num_children) {
    depth = d;
    num_children.resize(d);
    for (int i = 0; i < d; i++) {
        num_children[i] = _num_children[i];
        sum_children += num_children[i];
    }
    root = new InternalNode{true, std::vector<TrieNode*>(1 << num_children[0])};
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