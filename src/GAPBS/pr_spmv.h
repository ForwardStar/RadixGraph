//
// Created by sjf on 8/31/2022.
//

#ifndef GRAPHINDEX_PR_SPMV_H
#define GRAPHINDEX_PR_SPMV_H


#include <algorithm>
#include <iostream>
#include <vector>

#include "benchmark.h"
#include "builder.h"
#include "command_line.h"
#include "graph.h"
#include "pvector.h"
#include "../../../index_algorithms.h"
#include "../../../spruce_transaction.h"

typedef float ScoreT;
const float kDamp = 0.85;

pvector<ScoreT> PageRankPull(SpruceTransVer::TopBlock* g, int max_iters,
                             uint32_t num_nodes, double epsilon = 0);

void PrintTopScores(const Graph &g, const pvector<ScoreT> &scores);

bool PRVerifier(const Graph &g, const pvector<ScoreT> &scores,
                double target_error);
#endif //GRAPHINDEX_PR_SPMV_H
