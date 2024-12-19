#include "headers.h"

struct DummyNode {
    uint64_t node = -1; // this dummy node corresponds to which vertex
    uint8_t flag[max_number_of_threads];
    std::mutex mtx;
};

class Trie {
    public:
        struct TrieNode {
            int level;
        };

        typedef struct _internal_node : TrieNode {
            std::vector<TrieNode*> children;
            std::mutex mtx;
        } InternalNode;

        typedef struct _leaf_node : TrieNode {
            DummyNode* head = nullptr;
            std::mutex mtx;
        } LeafNode;

        InternalNode* root;
        std::vector<int> num_bits, sum_bits;
        int depth = 0;
        int space = 0;

        void InsertVertex(TrieNode* current, DummyNode* node);
        void InsertVertex(DummyNode* node);

        TrieNode* RetrieveVertex(uint64_t id, bool lock=false);

        long long size();

        Trie() {}
        Trie(int d, int _num_bits[]);
        Trie(int d, std::vector<int> _num_bits);
        ~Trie();
};