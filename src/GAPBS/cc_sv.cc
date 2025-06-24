// Copyright (c) 2015, The Regents of the University of California (Regents)
// See LICENSE.txt for license details

#include "cc_sv.h"


/*
GAP Benchmark Suite
Kernel: Connected Components (CC)
Author: Scott Beamer

Will return comp array labelling each vertex with a connected component ID

This CC implementation makes use of the Shiloach-Vishkin [2] algorithm with
implementation optimizations from Bader et al. [1]. Michael Sutton contributed
a fix for directed graphs using the min-max swap from [3], and it also produces
more consistent performance for undirected graphs.

[1] David A Bader, Guojing Cong, and John Feo. "On the architectural
    requirements for efficient execution of graph algorithms." International
    Conference on Parallel Processing, Jul 2005.

[2] Yossi Shiloach and Uzi Vishkin. "An o(logn) parallel connectivity algorithm"
    Journal of Algorithms, 3(1):57–67, 1982.

[3] Kishore Kothapalli, Jyothish Soman, and P. J. Narayanan. "Fast GPU
    algorithms for graph connectivity." Workshop on Large Scale Parallel
    Processing, 2010.
*/
// The hooking condition (comp_u < comp_v) may not coincide with the edge's
// direction, so we use a min-max swap such that lower component IDs propagate
// independent of the edge's direction.
pvector<NodeID> ShiloachVishkin(RadixGraph* g, uint32_t num_nodes) {
  pvector<NodeID> comp(num_nodes);
  #pragma omp parallel for
  for (NodeID n=0; n < num_nodes; n++) comp[n] = n;
  bool change = true;
  int num_iter = 0;
  while (change) {
    change = false;
    num_iter++;
    #pragma omp parallel for
    for (NodeID n=0; n < num_nodes; n++) {
      std::vector<WeightedEdge> neighbours;
      auto u = g->vertex_index->RetrieveVertex(n);
      if (u == nullptr) continue;
      g->GetNeighbours(u, neighbours);
      for (auto& e : neighbours) {
        NodeID comp_u = comp[n];
        NodeID comp_v = comp[e.idx];
        if (comp_u == comp_v) continue;
        // Hooking condition so lower component ID wins independent of direction
        NodeID high_comp = comp_u > comp_v ? comp_u : comp_v;
        NodeID low_comp = comp_u + (comp_v - high_comp);
        if (high_comp == comp[high_comp]) {
          change = true;
          comp[high_comp] = low_comp;
        }
      }
    }
    #pragma omp parallel for
    for (NodeID n=0; n < num_nodes; n++) {
      while (comp[n] != comp[comp[n]]) {
        comp[n] = comp[comp[n]];
      }
    }
  }
  return comp;
}