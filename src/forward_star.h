#ifndef FORWARDSTAR
#define FORWARDSTAR

#include "headers.h"

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

        typedef struct _dummy_node {
            uint64_t node; // this dummy node corresponds to which vertex
            bool obsolete; // whether this vertex has been visited in graph traversal task
            _weighted_edge* next; // linked-list-based edge structure
        } DummyNode;

        std::map<uint64_t, DummyNode*> vertex_index;
        std::vector<DummyNode*> dummy_nodes;

        bool InsertEdge(uint64_t src, uint64_t des, double weight);

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

        typedef struct _dummy_node {
            uint64_t node; // this dummy node corresponds to which vertex
            bool obsolete; // whether this vertex has been visited in graph traversal task
            std::vector<WeightedEdge> next; // array-based edge structure
        } DummyNode;

        std::map<uint64_t, DummyNode*> vertex_index;
        std::vector<DummyNode*> dummy_nodes;

        bool InsertEdge(uint64_t src, uint64_t des, double weight);

        void BFS(uint64_t src);

        ArrayForwardStar() {}
        ~ArrayForwardStar();
};

#endif