#include "headers.h"
#ifndef TRIE
#define TRIE

typedef struct _weighted_edge;
typedef struct _dummy_node;

typedef struct _weighted_edge {
    double weight; // edge property
    _dummy_node* forward; // forward pointer to dummy node of target vertex
    int timestamp;
    uint8_t flag;
} WeightedEdge;

typedef struct _dummy_node {
    uint64_t node = -1; // this dummy node corresponds to which vertex
    uint8_t flag[max_number_of_threads];
    WeightedEdge* next = nullptr;
    int cap = 0;
    std::atomic<int> cnt = 0;
    std::atomic<uint8_t> mtx = 0;

    ~_dummy_node() {
        delete [] next;
    }
} DummyNode;

class Trie {
    public:
        typedef struct _trie_node {
            DummyNode* head = nullptr;
            _trie_node* children = nullptr;
            std::atomic<uint8_t> mtx = 0;

            ~_trie_node() {
                if (children) {
                    delete [] children;
                }
                if (head) {
                    delete [] head;
                }
            }
        } TrieNode;

        TrieNode root;
        std::vector<int> num_bits, sum_bits;
        int depth = 0;
        int space = 0;

        inline DummyNode* InsertVertex(TrieNode* current, uint64_t id, int d);

        DummyNode* RetrieveVertex(uint64_t id, bool insert_mode=false);

        bool DeleteVertex(uint64_t id);

        long long size();

        Trie() {}
        Trie(int d, int _num_bits[]);
        Trie(int d, std::vector<int> _num_bits);
        ~Trie() {};
};

#endif