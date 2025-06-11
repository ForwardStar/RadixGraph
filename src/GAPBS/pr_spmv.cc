// Copyright (c) 2015, The Regents of the University of California (Regents)
// See LICENSE.txt for license details

#include "pr_spmv.h"

/*
GAP Benchmark Suite
Kernel: PageRank (PR)
Author: Scott Beamer

Will return pagerank scores for all vertices once total change < epsilon

This legacy PR implementation uses the traditional iterative approach. This is
done to ease comparisons to other implementations (often use same algorithm),
but it is not necesarily the fastest way to implement it. It performs each
iteration as a sparse-matrix vector multiply (SpMV), and values are not visible
until the next iteration (like Jacobi-style method).
*/
pvector<ScoreT> PageRankPull(RadixGraph* g, int max_iters,
                             uint32_t num_nodes, double epsilon) {

  const ScoreT init_score = 1.0f / num_nodes;
  const ScoreT base_score = (1.0f - kDamp) / num_nodes;
  pvector<ScoreT> scores(num_nodes, init_score);
  pvector<ScoreT> outgoing_contrib(num_nodes, 0.0);
  for (int iter=0; iter < max_iters; iter++) {
      double error = 0;
      double dangling_sum = 0.0;

      #pragma omp parallel for reduction(+:dangling_sum)
      for (NodeID n = 0; n < num_nodes; n++) {
          uint32_t out_degree = g->degree[n];
          if (out_degree == 0) {
              dangling_sum += scores[n];
          }
          else {
              outgoing_contrib[n] = scores[n] / out_degree;
          }
      }

      dangling_sum /= num_nodes;
      #pragma omp parallel for schedule(guided)
      for (NodeID n = 0; n < num_nodes; n++) {
        ScoreT incoming_total = 0;
        std::vector<WeightedEdge> neighbours;
        g->GetNeighboursByOffset(n, neighbours);
        for (auto e : neighbours) {
          incoming_total += outgoing_contrib[e.idx];
        }
        scores[n] = base_score + kDamp * (incoming_total + dangling_sum);
      }
  }
  return scores;
}