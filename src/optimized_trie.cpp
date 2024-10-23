#include "optimized_trie.h"

void Trie::InsertVertex(uint64_t id, DummyNode* node) {
    int num_bits = sum_children;
    TrieNode* current = root;
    while (true) {
        if (current->is_internal) {
            InternalNode* tmp = (InternalNode*)current;
            if (tmp->children.size() > 0) {
                uint64_t idx = ((id & ((1ull << num_bits) - 1)) >> (num_bits - num_children[tmp->level]));
                if (tmp->children[idx] == nullptr) {
                    if (tmp->level < depth - 1) {
                        tmp->children[idx] = new InternalNode{true, tmp->level + 1, std::vector<TrieNode*>()};
                    }
                    else {
                        tmp->children[idx] = new LeafNode{false, nullptr};
                    }
                }
                current = tmp->children[idx];
                num_bits -= num_children[tmp->level];
            }
            else {
                tmp->children.resize(1 << num_children[tmp->level]);
                uint64_t idx = ((id & ((1ull << num_bits) - 1)) >> (num_bits - num_children[tmp->level]));
                if (tmp->level < depth - 1) {
                    tmp->children[idx] = new InternalNode{true, tmp->level + 1, std::vector<TrieNode*>()};
                }
                else {
                    tmp->children[idx] = new LeafNode{false, nullptr};
                }
                current = tmp->children[idx];
                num_bits -= num_children[tmp->level];
            }
        }
        else {
            LeafNode* tmp = (LeafNode*)current;
            tmp->head = node;
            break;
        }
    }
}

DummyNode* Trie::RetrieveVertex(uint64_t id) {
    int num_bits = sum_children;
    TrieNode* current = root;
    while (true) {
        if (current == nullptr) {
            break;
        }
        if (current->is_internal) {
            InternalNode* tmp = (InternalNode*)current;
            if (tmp->children.size() > 0) {
                uint64_t idx = ((id & ((1ull << num_bits) - 1)) >> (num_bits - num_children[tmp->level]));
                current = tmp->children[idx];
                num_bits -= num_children[tmp->level];
            }
            else {
                break;
            }
        }
        else {
            LeafNode* tmp = (LeafNode*)current;
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
    root = new InternalNode{true, 0, std::vector<TrieNode*>()};
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