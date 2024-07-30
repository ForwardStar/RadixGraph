#include "headers.h"

class forward_star {
    private:
    public:
        typedef struct _weighted_edge {
            uint64_t src, des;
            bool deleted;
            double weight;
            _weighted_edge* forward;
            _weighted_edge* next;
        } WeightedEdge;

        std::map<uint64_t, WeightedEdge*> vertex_index;
};