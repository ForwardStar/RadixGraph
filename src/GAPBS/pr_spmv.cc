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


using namespace std;



pvector<ScoreT> PageRankPull(SpruceTransVer::TopBlock* g, int max_iters,
                             uint32_t num_nodes, double epsilon) {

  const ScoreT init_score = 1.0f / /*g.num_nodes()*/num_nodes;
  const ScoreT base_score = (1.0f - kDamp) / /*g.num_nodes()*/num_nodes;
  pvector<ScoreT> scores(/*g.num_nodes()*/num_nodes, init_score);
  pvector<ScoreT> outgoing_contrib(num_nodes, 0.0);
  double middle, start, end; // time analysis
//  start = omp_get_wtime();
  for (int iter=0; iter < max_iters; iter++) {
      double error = 0;
      double dangling_sum = 0.0;


#pragma omp parallel for reduction(+:dangling_sum)
      for (NodeID n = 0; n < num_nodes; n++) {
          uint32_t out_degree = SpruceTransVer::GetDegree(g, n);
          if (out_degree == 0) {
              dangling_sum += scores[n];
          }
          else {
              outgoing_contrib[n] = scores[n] / out_degree;
          }
      }

//      middle = omp_get_wtime();

      dangling_sum /= num_nodes;
    #pragma omp parallel for /*reduction(+ : error)*/ schedule(guided/*dynamic, 64*/)
    for (NodeID u=0; u < num_nodes; u++) {
      ScoreT incoming_total = 0;
        std::vector<uint32_t> neighbours;
//        uint32_t f = (uint32_t)u;
        SpruceTransVer::get_neighbours_only(g, u, neighbours);
      for (auto v : neighbours)
        incoming_total += outgoing_contrib[v];
//      ScoreT old_score = scores[u];
      scores[u] = base_score + kDamp * (incoming_total + dangling_sum);
//      error += fabs(scores[u] - old_score);
    }
//    printf(" %2d    %lf\n", iter, error);
//    if (error < epsilon)
//      break;
  }
//  end = omp_get_wtime();
//  printf("Time to get degree: %lf s \n Time total: %lf s \n", middle - start, end - start);
  return scores;
}


void PrintTopScores(const Graph &g, const pvector<ScoreT> &scores) {
  vector<pair<NodeID, ScoreT>> score_pairs(g.num_nodes());
  for (NodeID n=0; n < g.num_nodes(); n++) {
    score_pairs[n] = make_pair(n, scores[n]);
  }
  int k = 5;
  vector<pair<ScoreT, NodeID>> top_k = TopK(score_pairs, k);
  k = min(k, static_cast<int>(top_k.size()));
  for (auto kvp : top_k)
    cout << kvp.second << ":" << kvp.first << endl;
}


// Verifies by asserting a single serial iteration in push direction has
//   error < target_error
bool PRVerifier(const Graph &g, const pvector<ScoreT> &scores,
                        double target_error) {
  const ScoreT base_score = (1.0f - kDamp) / g.num_nodes();
  pvector<ScoreT> incomming_sums(g.num_nodes(), 0);
  double error = 0;
  for (NodeID u : g.vertices()) {
    ScoreT outgoing_contrib = scores[u] / g.out_degree(u);
    for (NodeID v : g.out_neigh(u))
      incomming_sums[v] += outgoing_contrib;
  }
  for (NodeID n : g.vertices()) {
    error += fabs(base_score + kDamp * incomming_sums[n] - scores[n]);
    incomming_sums[n] = 0;
  }
  PrintTime("Total Error", error);
  return error < target_error;
}


//int main(int argc, char* argv[]) {
//  CLPageRank cli(argc, argv, "pagerank", 1e-4, 20);
//  if (!cli.ParseArgs())
//    return -1;
//  Builder b(cli);
//  Graph g = b.MakeGraph();
//  auto PRBound = [&cli] (const Graph &g) {
//    return PageRankPull(g, cli.max_iters(), cli.tolerance());
//  };
//  auto VerifierBound = [&cli] (const Graph &g, const pvector<ScoreT> &scores) {
//    return PRVerifier(g, scores, cli.tolerance());
//  };
//  BenchmarkKernel(cli, g, PRBound, PrintTopScores, VerifierBound);
//  return 0;
//}
