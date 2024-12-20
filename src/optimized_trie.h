#include "headers.h"

struct DummyNode {
    uint64_t node = -1; // this dummy node corresponds to which vertex
    uint8_t flag[max_number_of_threads];
    std::mutex mtx;
};

class Trie {
    public:
        typedef struct _trie_node {
            int level = -1;
            DummyNode* head = nullptr;
            _trie_node* children = nullptr;
            std::mutex mtx;

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

        void InsertVertex(TrieNode* current, DummyNode* u_ptr);
        void InsertVertex(DummyNode* u_ptr);

        TrieNode* RetrieveVertex(uint64_t id, bool lock=false);

        long long size();

        Trie() {}
        Trie(int d, int _num_bits[]);
        Trie(int d, std::vector<int> _num_bits);
        ~Trie();
};