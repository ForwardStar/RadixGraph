#ifndef FORWARDSTAR
#define FORWARDSTAR

#include "headers.h"
#include "optimized_trie.h"

class ForwardStar {
    private:
    public:
        typedef struct _weighted_edge;
        typedef struct _dummy_node;

        typedef struct _weighted_edge {
            double weight; // edge property
            _dummy_node* forward; // forward pointer to dummy node of target vertex
            uint8_t flag;
        } WeightedEdge;

        typedef struct _dummy_node : DummyNode {
            tbb::concurrent_vector<WeightedEdge> next; // array-based edge structure
        } DummyNode;

        Trie* vertex_index = nullptr;
        std::atomic<int> num_dummy_nodes, cnt;
        std::vector<int> thread_pool;
        std::vector<DummyNode*> dummy_nodes;
        std::mutex mtx;

        void ExpandDummies();

        inline DummyNode* RetrieveOrInsert(uint64_t u);

        bool InsertEdge(uint64_t src, uint64_t des, double weight);

        bool UpdateEdge(uint64_t src, uint64_t des, double weight);

        bool DeleteEdge(uint64_t src, uint64_t des);

        bool GetNeighbours(uint64_t src, std::vector<WeightedEdge> &neighbours);

        bool GetNeighbours(DummyNode* src, std::vector<WeightedEdge> &neighbours);

        std::vector<DummyNode*> BFS(uint64_t src);

        ForwardStar(int d, std::vector<int> _num_children);
        ~ForwardStar();
};

#endif