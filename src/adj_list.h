#ifndef ADJLIST
#define ADJLIST

#include "headers.h"

class AdjacencyList {
    private:
    public:
        typedef struct _weighted_edge {
            uint64_t des;
            double weight;
            _weighted_edge* next;
        } WeightedEdge;

        std::map<uint64_t, WeightedEdge*> vertex_index;

        bool InsertEdge(uint64_t src, uint64_t des, double weight);

        void BFS(uint64_t src);

        AdjacencyList() {}
        ~AdjacencyList();
};

#endif