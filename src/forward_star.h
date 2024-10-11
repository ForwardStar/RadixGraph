#ifndef FORWARDSTAR
#define FORWARDSTAR

#include "headers.h"
#include "optimized_trie.h"

class ChainedForwardStar {
    private:
    public:
        typedef struct _weighted_edge;
        typedef struct _dummy_node;

        typedef struct _weighted_edge {
            double weight; // edge property
            _dummy_node* forward; // forward pointer to dummy node of target vertex
            _weighted_edge* next; // next edge for this source vertex
        } WeightedEdge;

        typedef struct _dummy_node : DummyNode {
            _weighted_edge* next; // linked-list-based edge structure
        } DummyNode;

        std::map<uint64_t, DummyNode*> vertex_index;
        std::vector<DummyNode*> dummy_nodes;

        bool InsertEdge(uint64_t src, uint64_t des, double weight);

        bool UpdateEdge(uint64_t src, uint64_t des, double weight);

        bool DeleteEdge(uint64_t src, uint64_t des);

        bool GetNeighbours(uint64_t src, std::vector<WeightedEdge*> &neighbours);

        bool GetNeighbours(DummyNode* src, std::vector<WeightedEdge*> &neighbours);

        void BFS(uint64_t src);

        ChainedForwardStar() {}
        ~ChainedForwardStar();
};

class ArrayForwardStar {
    private:
    public:
        typedef struct _weighted_edge;
        typedef struct _dummy_node;

        typedef struct _weighted_edge {
            double weight; // edge property
            _dummy_node* forward; // forward pointer to dummy node of target vertex
        } WeightedEdge;

        typedef struct _dummy_node : DummyNode {
            std::vector<WeightedEdge*> next; // array-based edge structure
        } DummyNode;

        std::map<uint64_t, DummyNode*> vertex_index;
        std::vector<DummyNode*> dummy_nodes;

        bool InsertEdge(uint64_t src, uint64_t des, double weight);

        bool UpdateEdge(uint64_t src, uint64_t des, double weight);

        bool DeleteEdge(uint64_t src, uint64_t des);

        bool GetNeighbours(uint64_t src, std::vector<WeightedEdge*> &neighbours);

        bool GetNeighbours(DummyNode* src, std::vector<WeightedEdge*> &neighbours);

        void BFS(uint64_t src);

        ArrayForwardStar() {}
        ~ArrayForwardStar();
};

#endif