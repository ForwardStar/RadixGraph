#ifndef FORWARDSTAR
#define FORWARDSTAR

#include "headers.h"

class ForwardStar {
    private:
    public:
        typedef struct _weighted_edge {
            uint64_t src, des;
            bool deleted;
            double weight;
            _weighted_edge* forward;
            _weighted_edge* next;
        } WeightedEdge;

        std::map<uint64_t, std::pair<WeightedEdge*, WeightedEdge*>> vertex_index;

        bool InsertEdge(uint64_t src, uint64_t des, double weight);
};

#endif