//
// Created by sjf on 9/7/2022.
//
// Weakly connected components

#ifndef GRAPHINDEX_CC_SV_H
#define GRAPHINDEX_CC_SV_H

#include <algorithm>
#include <cinttypes>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "benchmark.h"
#include "bitmap.h"
#include "builder.h"
#include "command_line.h"
#include "graph.h"
#include "pvector.h"
#include "timer.h"
#include "../../../index_algorithms.h"
#include "../../../spruce_transaction.h"

pvector<NodeID> ShiloachVishkin(SpruceTransVer::TopBlock* g, uint32_t num_nodes);

void PrintCompStats(const Graph &g, const pvector<NodeID> &comp);

bool CCVerifier(const Graph &g, const pvector<NodeID> &comp);
#endif //GRAPHINDEX_CC_SV_H
