#ifndef FORWARDSTAR
#define FORWARDSTAR

#include "headers.h"

class ForwardStar {
    private:
    public:
        typedef struct _weighted_edge;
        typedef struct _dummy_node;

        typedef struct _weighted_edge {
            uint64_t des;
            double weight;
            _dummy_node* forward;
            _weighted_edge* next;
        } WeightedEdge;

        typedef struct _dummy_node {
            bool obsolete;
            _weighted_edge* next;
        } DummyNode;

        std::map<uint64_t, DummyNode*> vertex_index;
        std::vector<DummyNode*> dummy_nodes;

        bool InsertEdge(uint64_t src, uint64_t des, double weight);

        void BFS(uint64_t src);

        ForwardStar() {}
        ~ForwardStar();
};

#endif