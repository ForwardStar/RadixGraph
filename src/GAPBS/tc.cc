// Copyright (c) 2015, The Regents of the University of California (Regents)
// See LICENSE.txt for license details

// Encourage use of gcc's parallel algorithms (for sort for relabeling)

#include "tc.h"

/*
GAP Benchmark Suite
Kernel: Triangle Counting (TC)
Author: Scott Beamer

Will count the number of triangles (cliques of size 3)

Requires input graph:
  - to be undirected
  - no duplicate edges (or else will be counted as multiple triangles)
  - neighborhoods are sorted by vertex identifiers

Other than symmetrizing, the rest of the requirements are done by SquishCSR
during graph building.

This implementation reduces the search space by counting each triangle only
once. A naive implementation will count the same triangle six times because
each of the three vertices (u, v, w) will count it in both ways. To count
a triangle only once, this implementation only counts a triangle if u > v > w.
Once the remaining unexamined neighbors identifiers get too big, it can break
out of the loop, but this requires that the neighbors to be sorted.

Another optimization this implementation has is to relabel the vertices by
degree. This is beneficial if the average degree is high enough and if the
degree distribution is sufficiently non-uniform. To decide whether or not
to relabel the graph, we use the heuristic in WorthRelabelling.
*/
size_t OrderedCount(RadixGraph* g, uint32_t num_vertices) {
  size_t total = 0;
  bool is_sorted = g->is_sorted;
  #pragma omp parallel for reduction(+ : total) schedule(dynamic, 64)
  for (NodeID u=0; u < num_vertices; u++) {
    std::vector<WeightedEdge> neighbours;
    #if USE_EDGE_CHAIN
      g->GetNeighboursByOffset(u, neighbours);
    #else
      g->GetNeighbours(g->vertex_index->vertex_table[u].node, neighbours);
    #endif
    if (!is_sorted) {
      std::sort(neighbours.begin(), neighbours.end(), [](const WeightedEdge &a, const WeightedEdge &b) {
        return a.idx < b.idx;
      });
    }
    for (auto& e : neighbours) {
      NodeID v = e.idx;
      if (v > u)
        break;
      std::vector<WeightedEdge> v_neighbours;
      #if USE_EDGE_CHAIN
        g->GetNeighboursByOffset(v, v_neighbours);
      #else
        g->GetNeighbours(g->vertex_index->vertex_table[v].node, v_neighbours);
      #endif
      if (!is_sorted) {
        std::sort(v_neighbours.begin(), v_neighbours.end(), [](const WeightedEdge &a, const WeightedEdge &b) {
          return a.idx < b.idx;
        });
      }
      int i = 0;
      for (auto& f : neighbours) {
        NodeID w = f.idx;
        if (w > v)
          break;
        while (i < v_neighbours.size() && v_neighbours[i].idx < w)
          i++;
        if (i < v_neighbours.size() && w == v_neighbours[i].idx)
          total++;
      }
    }
  }
  return total;
}