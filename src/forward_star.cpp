#include "forward_star.h"

bool ForwardStar::InsertEdge(uint64_t src, uint64_t des, double weight) {
    if (vertex_index.find(src) == vertex_index.end()) {
        auto tmp = new WeightedEdge{0, 0, 0, 0, nullptr, nullptr};
        vertex_index[src] = std::make_pair(tmp, tmp);
    }
    if (vertex_index.find(des) == vertex_index.end()) {
        auto tmp = new WeightedEdge{0, 0, 0, 0, nullptr, nullptr};
        vertex_index[des] = std::make_pair(tmp, tmp);
    }

    auto tmp = vertex_index[src].second;
    tmp->next = new WeightedEdge{src, des, false, weight, vertex_index[des].first, nullptr};
}