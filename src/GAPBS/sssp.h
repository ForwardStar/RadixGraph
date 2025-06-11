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
#include "platform_atomics.h"
#include "pvector.h"
#include "../radixgraph.h"

inline void RelaxEdges(RadixGraph* g, int u, WeightT delta,
                pvector<WeightT> &dist, std::vector<std::vector<int>> &local_bins) {
    std::vector<WeightedEdge> neighbours;
    g->GetNeighboursByOffset(u, neighbours);
    for (auto e : neighbours) {
        int vidx = e.idx;
        WeightT old_dist = dist[vidx];
        WeightT new_dist = dist[u] + e.weight;
        while (new_dist < old_dist) {
            if (compare_and_swap(dist[vidx], old_dist, new_dist)) {
                size_t dest_bin = new_dist / delta;
                if (dest_bin >= local_bins.size())
                    local_bins.resize(dest_bin + 1);
                local_bins[dest_bin].push_back(vidx);
                break;
            }
            old_dist = dist[vidx];      // swap failed, recheck dist update & retry
        }
    }
}

pvector<WeightT> DeltaStep(RadixGraph* g, NodeID source, WeightT delta, uint32_t num_nodes, long num_edges);

#endif //GRAPHINDEX_SSSP_H
