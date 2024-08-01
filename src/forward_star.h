#ifndef FORWARDSTAR
#define FORWARDSTAR

#include "headers.h"

class ForwardStar {
    private:
    public:
        typedef struct _weighted_edge {
            uint64_t src, des;
            double weight;
            bool visited;
            _weighted_edge* forward;
            _weighted_edge* next;
        } WeightedEdge;

        std::map<uint64_t, std::pair<WeightedEdge*, WeightedEdge*>> vertex_index;
        std::vector<WeightedEdge*> head_edges;

        bool InsertEdge(uint64_t src, uint64_t des, double weight);

        void BFS(uint64_t src);

        ForwardStar() {}
        ~ForwardStar();
};

#endif