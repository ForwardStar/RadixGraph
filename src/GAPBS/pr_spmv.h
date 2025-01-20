//
// Created by sjf on 8/31/2022.
//

#ifndef GRAPHINDEX_PR_SPMV_H
#define GRAPHINDEX_PR_SPMV_H


#include <algorithm>
#include <iostream>
#include <vector>

#include "benchmark.h"
#include "command_line.h"
#include "pvector.h"
#include "../forward_star.h"

typedef float ScoreT;
const float kDamp = 0.85;

pvector<ScoreT> PageRankPull(ForwardStar* g, int max_iters,
                             uint32_t num_nodes, double epsilon = 0);

#endif //GRAPHINDEX_PR_SPMV_H
