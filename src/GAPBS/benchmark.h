// Copyright (c) 2015, The Regents of the University of California (Regents)
// See LICENSE.txt for license details

#ifndef BENCHMARK_H_
#define BENCHMARK_H_

#include <algorithm>
#include <cinttypes>
#include <utility>

/*
GAP Benchmark Suite
File:   Benchmark
Author: Scott Beamer

Various helper functions to ease writing of kernels
*/

// Default type signatures for commonly used types
typedef uint64_t NodeID;     //use int_32_t for BFS, uint32_t for others
typedef double WeightT;

#endif