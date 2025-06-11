//
// Created by sjf on 8/31/2022.
//

#ifndef GRAPHINDEX_PR_SPMV_H
#define GRAPHINDEX_PR_SPMV_H

#include "benchmark.h"
#include "pvector.h"
#include "../radixgraph.h"

typedef float ScoreT;
const float kDamp = 0.85;

pvector<ScoreT> PageRankPull(RadixGraph* g, int max_iters,
                             uint32_t num_nodes, double epsilon = 0);

#endif //GRAPHINDEX_PR_SPMV_H
