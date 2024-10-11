#include "optimized_trie.h"

void Trie::InsertVertex(uint64_t id, DummyNode* node) {
    int num_bits = sum_children;
    TrieNode current = root;
    while (true) {
        if (current.is_internal) {
            InternalNode tmp = (InternalNode)current;
            if (tmp.children) {
                uint64_t idx = (((1ull << num_bits) - 1) >> (num_bits - num_children[tmp.level]));
                current = tmp.children[idx];
                num_bits -= num_children[tmp.level];
            }
            else {
                if (tmp.level < depth - 1) {
                    tmp.children = new InternalNode[num_children[tmp.level]](true, tmp.level + 1, nullptr);
                }
                else {
                    tmp.children = new LeafNode[num_children[tmp.level]](false, nullptr);
                }
                uint64_t idx = (((1ull << num_bits) - 1) >> (num_bits - num_children[tmp.level]));
                current = tmp.children[idx];
                num_bits -= num_children[tmp.level];
            }
        }
        else {
            LeafNode tmp = (LeafNode)current;
            tmp.head = node;
        }
    }
}

DummyNode* Trie::RetrieveVertex(uint64_t id) {
    int num_bits = sum_children;
    TrieNode current = root;
    while (true) {
        if (current.is_internal) {
            InternalNode tmp = (InternalNode)current;
            if (tmp.children) {
                uint64_t idx = (((1ull << num_bits) - 1) >> (num_bits - num_children[tmp.level]));
                current = tmp.children[idx];
                num_bits -= num_children[tmp.level];
            }
            else {
                break;
            }
        }
        else {
            LeafNode tmp = (LeafNode)current;
            return tmp.head;
        }
    }
    return nullptr;
}

Trie::Trie(int d, int _num_children[]) {
    depth = d;
    num_children = new int[depth];
    for (int i = 0; i < d; i++) {
        num_children[i] = _num_children[i];
        sum_children += num_children[i];
    }
    root = InternalNode{true, 0, nullptr};
}

Trie::~Trie() {
    std::stack<std::pair<TrieNode, bool>> S;
    S.push({root, false});
    while (!S.empty()) {
        TrieNode current;
        bool flag;
        std::tie(current, flag) = S.top();
        S.pop();
        if (!flag) {
            S.push({current, true});
            if (current.is_internal) {
                InternalNode tmp = (InternalNode)current;
                if (tmp.children) {
                    for (int i = 0; i < num_children[tmp.level]; i++) {
                        S.push({tmp.children[i], false});
                    }
                }
            }
        }
        else {
            if (current.is_internal) {
                InternalNode tmp = (InternalNode)current;
                if (tmp.children) {
                    delete [] tmp.children;
                }
            }
        }
        
    }
    delete [] num_children;
}