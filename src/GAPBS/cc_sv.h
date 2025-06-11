//
// Created by sjf on 9/7/2022.
//
// Weakly connected components

#ifndef GRAPHINDEX_CC_SV_H
#define GRAPHINDEX_CC_SV_H

#include "benchmark.h"
#include "bitmap.h"
#include "pvector.h"
#include "../radixgraph.h"

pvector<NodeID> ShiloachVishkin(RadixGraph* g, uint32_t num_nodes);

#endif //GRAPHINDEX_CC_SV_H
