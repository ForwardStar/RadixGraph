//
// Created by sjf on 9/7/2022.
//

#ifndef GRAPHINDEX_SSSP_H
#define GRAPHINDEX_SSSP_H

#include <cinttypes>
#include <limits>
#include <iostream>
#include <queue>
#include <vector>

#include "benchmark.h"
#include "command_line.h"
#include "platform_atomics.h"
#include "pvector.h"
#include "../forward_star.h"

inline void RelaxEdges(ForwardStar* g, DummyNode* u, WeightT delta,
                pvector<WeightT> &dist, std::vector<std::vector<DummyNode*>> &local_bins) {
    std::vector<WeightedEdge> neighbours;
    g->GetNeighbours(u, neighbours);
    for (auto e : neighbours) {
        auto v = e.forward;
        WeightT old_dist = dist[v->idx];
        WeightT new_dist = dist[u->idx] + e.weight;
        while (new_dist < old_dist) {
            if (compare_and_swap(dist[v->idx], old_dist, new_dist)) {
                size_t dest_bin = new_dist / delta;
                if (dest_bin >= local_bins.size())
                    local_bins.resize(dest_bin + 1);
                local_bins[dest_bin].push_back(v);
                break;
            }
            old_dist = dist[v->idx];      // swap failed, recheck dist update & retry
        }
    }
}

pvector<WeightT> DeltaStep(ForwardStar* g, NodeID source, WeightT delta, uint32_t num_nodes, long num_edges);

#endif //GRAPHINDEX_SSSP_H
