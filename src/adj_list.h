#ifndef ADJLIST
#define ADJLIST

#include "headers.h"

class AdjacencyLinkedList {
    private:
    public:
        typedef struct _weighted_edge {
            uint64_t des; // target vertex id
            double weight; // edge property
            _weighted_edge* next; // next edge for this source vertex
        } WeightedEdge;

        std::map<uint64_t, WeightedEdge*> vertex_index;

        bool InsertEdge(uint64_t src, uint64_t des, double weight);

        bool UpdateEdge(uint64_t src, uint64_t des, double weight);

        bool DeleteEdge(uint64_t src, uint64_t des);

        bool GetNeighbours(uint64_t src, std::vector<WeightedEdge*> &neighbours);

        void BFS(uint64_t src);

        AdjacencyLinkedList() {}
        ~AdjacencyLinkedList();
};

class AdjacencyArrayList {
    private:
    public:
        typedef struct _weighted_edge {
            uint64_t des; // target vertex id
            double weight; // edge property
        } WeightedEdge;

        std::map<uint64_t, std::vector<WeightedEdge*>> vertex_index;

        bool InsertEdge(uint64_t src, uint64_t des, double weight);

        bool UpdateEdge(uint64_t src, uint64_t des, double weight);

        bool DeleteEdge(uint64_t src, uint64_t des);

        bool GetNeighbours(uint64_t src, std::vector<WeightedEdge*> &neighbours);

        void BFS(uint64_t src);

        AdjacencyArrayList() {}
        ~AdjacencyArrayList();
};

#endif