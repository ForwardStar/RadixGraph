//
// Created by sjf on 9/2/2022.
//

#ifndef GRAPHINDEX_TC_H
#define GRAPHINDEX_TC_H

#ifdef _OPENMP
    #define _GLIBCXX_PARALLEL
#endif

#include <algorithm>
#include <cinttypes>
#include <iostream>
#include <vector>

#include "benchmark.h"
#include "builder.h"
#include "command_line.h"
#include "graph.h"
#include "pvector.h"
#include "../../../index_algorithms.h"
#include "../../../spruce_transaction.h"

std::vector<double> OrderedCount(SpruceTransVer::TopBlock* g, uint32_t num_vertices);

bool WorthRelabelling(const Graph &g);

size_t Hybrid(const Graph &g);

void PrintTriangleStats(const Graph &g, size_t total_triangles);

bool TCVerifier(const Graph &g, size_t test_total);

#endif //GRAPHINDEX_TC_H
