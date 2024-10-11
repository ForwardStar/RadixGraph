#include "headers.h"

struct DummyNode {
    uint64_t node; // this dummy node corresponds to which vertex
    uint8_t flag;
};

class Trie {
    private:
        struct TrieNode {
            bool is_internal;
        };

        typedef struct _internal_node : TrieNode {
            int level;
            TrieNode* children;
        } InternalNode;

        typedef struct _leaf_node : TrieNode {
            DummyNode* head;
        } LeafNode;

    public:
        InternalNode root;
        int* num_children;
        int sum_children = 0;
        int depth = 0;
        int space = 0;

        void InsertVertex(uint64_t id, DummyNode* node);

        DummyNode* RetrieveVertex(uint64_t id);

        Trie(int d, int _num_children[]);
        ~Trie();
};