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
std::vector<double> OrderedCount(RadixGraph* g, uint32_t num_vertices) {
  size_t total = 0;
  auto triangles_per_vertex = (std::atomic<uint32_t>*)malloc(sizeof(std::atomic<uint32_t>) * num_vertices);
  #pragma omp parallel
  {
    auto triangles_u = 0u;
    auto triangles_v = 0u;

    #pragma omp for schedule(dynamic, 256)
    for (NodeID n = 0; n < num_vertices; n++) {
        std::vector<WeightedEdge> u_neighbours;
        g->GetNeighboursByOffset(n, u_neighbours);
        std::sort(u_neighbours.begin(), u_neighbours.end(), [](WeightedEdge a, WeightedEdge b) {
          return a.idx < b.idx;
        });
        for (auto e : u_neighbours) {
            auto v = e.idx;
            if (v > n) {
                break;
            }
            auto it = u_neighbours.begin();
            std::vector<WeightedEdge> v_neighbours;
            g->GetNeighboursByOffset(v, v_neighbours);
            std::sort(v_neighbours.begin(), v_neighbours.end(), [](WeightedEdge a, WeightedEdge b) {
              return a.idx < b.idx;
            });
            for (auto e1 : v_neighbours) {
                auto w = e1.idx;
                if (w > v) {
                    break;
                }
                while (it->idx < w) {
                    it++;
                }
                if (w == it->idx) {
                    triangles_u += 2;
                    triangles_v += 2;
                    triangles_per_vertex[e1.idx] += 2;
                }
            }
            triangles_per_vertex[v] += triangles_v;
            triangles_v = 0;
        }
        triangles_per_vertex[n] += triangles_u;
        triangles_u = 0;
    }
  }

  std::vector<double> lcc_values(num_vertices);
  #pragma omp parallel for
  for (NodeID v = 0; v < num_vertices; v++) {
      uint32_t degree = g->vertex_index->vertex_table[v].next.load()->checkpoint_deg.load();
      uint64_t max_num_edges = degree * (degree - 1);
      if (max_num_edges != 0) {
          lcc_values[v] = ((double) triangles_per_vertex[v]) / max_num_edges;
      } else {
          lcc_values[v] = 0.0;
      }
  }
  return lcc_values;
}