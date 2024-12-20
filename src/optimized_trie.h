#include "headers.h"
#ifndef TRIE
#define TRIE

struct DummyNode {
    uint64_t node = -1; // this dummy node corresponds to which vertex
    uint8_t flag[max_number_of_threads];
};

class Trie {
    public:
        typedef struct _trie_node {
            DummyNode* head = nullptr;
            _trie_node* children = nullptr;
            std::atomic<uint8_t> mtx;

            ~_trie_node() {
                if (children) {
                    delete [] children;
                }
                if (head) {
                    delete head;
                }
            }
        } TrieNode;

        TrieNode root;
        std::vector<int> num_bits, sum_bits;
        int depth = 0;
        int space = 0;

        TrieNode* InsertVertex(TrieNode* current, uint64_t u, int d);

        TrieNode* RetrieveVertex(uint64_t id, bool insert_mode=false);

        long long size();

        Trie() {}
        Trie(int d, int _num_bits[]);
        Trie(int d, std::vector<int> _num_bits);
        ~Trie() {};
};

#endif