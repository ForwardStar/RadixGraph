// Copyright (c) 2015, The Regents of the University of California (Regents)
// See LICENSE.txt for license details

#include "bc.h"

/*
GAP Benchmark Suite
Kernel: Betweenness Centrality (BC)
Author: Scott Beamer

Will return array of approx betweenness centrality scores for each vertex

This BC implementation makes use of the Brandes [1] algorithm with
implementation optimizations from Madduri et al. [2]. It is only approximate
because it does not compute the paths from every start vertex, but only a small
subset of them. Additionally, the scores are normalized to the range [0,1].

As an optimization to save memory, this implementation uses a Bitmap to hold
succ (list of successors) found during the BFS phase that are used in the back-
propagation phase.

[1] Ulrik Brandes. "A faster algorithm for betweenness centrality." Journal of
    Mathematical Sociology, 25(2):163–177, 2001.

[2] Kamesh Madduri, David Ediger, Karl Jiang, David A Bader, and Daniel
    Chavarria-Miranda. "A faster parallel algorithm and efficient multithreaded
    implementations for evaluating betweenness centrality on massive datasets."
    International Symposium on Parallel & Distributed Processing (IPDPS), 2009.
*/

void PBFS(RadixGraph *g, NodeID source, pvector<CountT> &path_counts,
    Bitmap &succ, std::vector<SlidingQueue<NodeID>::iterator> &depth_index,
    SlidingQueue<NodeID> &queue, uint32_t num_nodes) {
  pvector<NodeID> depths(num_nodes, -1);
  std::vector<std::atomic<bool>> occurred(num_nodes);
  depths[source] = 0;
  path_counts[source] = 1;
  queue.push_back(source);
  depth_index.push_back(queue.begin());
  queue.slide_window();
  #pragma omp parallel
  {
    NodeID depth = 0;
    QueueBuffer<NodeID> lqueue(queue);
    while (!queue.empty()) {
      depth++;
      #pragma omp for schedule(dynamic, 64) nowait
      for (auto q_iter = queue.begin(); q_iter < queue.end(); q_iter++) {
        NodeID u = *q_iter;
        std::vector<WeightedEdge> u_neighbours;
        #if USE_EDGE_CHAIN
          g->GetNeighboursByOffset(u, u_neighbours);
        #else
          g->GetNeighbours(g->vertex_index[u].node, u_neighbours);
        #endif
        for (auto &e : u_neighbours) {
          NodeID v = e.idx;
          if ((depths[v] == -1) &&
              (compare_and_swap(depths[v], static_cast<NodeID>(-1), depth))) {
            bool first_time = !occurred[v].exchange(true);
            if (first_time) {
                // Yet normally this is unnecessary but I don't know the compare_and_swap sometimes works malfunctionally...
                lqueue.push_back(v);
            }
          }
          if (depths[v] == depth) {
            succ.set_bit_atomic(v);
            #pragma omp atomic
            path_counts[v] += path_counts[u];
          }
        }
      }
      lqueue.flush();
      #pragma omp barrier
      #pragma omp single
      {
        depth_index.push_back(queue.begin());
        queue.slide_window();
      }
    }
  }
  depth_index.push_back(queue.begin());
}


pvector<ScoreT> Brandes(RadixGraph *g, NodeID num_iters, uint32_t num_nodes) {
  pvector<ScoreT> scores(num_nodes, 0);
  pvector<CountT> path_counts(num_nodes);
  Bitmap succ(num_nodes);
  std::vector<SlidingQueue<NodeID>::iterator> depth_index;
  SlidingQueue<NodeID> queue(num_nodes);
  NodeID sp = 0;
  for (NodeID iter=0; iter < num_iters; iter++) {
    NodeID source = sp++ % num_nodes;
    path_counts.fill(0);
    depth_index.resize(0);
    queue.reset();
    succ.reset();
    PBFS(g, source, path_counts, succ, depth_index, queue, num_nodes);
    pvector<ScoreT> deltas(num_nodes, 0);
    for (int d=depth_index.size()-2; d >= 0; d--) {
      #pragma omp parallel for schedule(dynamic, 64)
      for (auto it = depth_index[d]; it < depth_index[d+1]; it++) {
        NodeID u = *it;
        ScoreT delta_u = 0;
        std::vector<WeightedEdge> g_out_neigh;
        #if USE_EDGE_CHAIN
          g->GetNeighboursByOffset(u, g_out_neigh);
        #else
          g->GetNeighbours(g->vertex_index[u].node, g_out_neigh);
        #endif
        for (auto &e : g_out_neigh) {
          NodeID v = e.idx;
          if (succ.get_bit(v)) {
            delta_u += (path_counts[u] / path_counts[v]) * (1 + deltas[v]);
          }
        }
        deltas[u] = delta_u;
        scores[u] += delta_u;
      }
    }
  }
  // normalize scores
  ScoreT biggest_score = 0;
  #pragma omp parallel for reduction(max : biggest_score)
  for (NodeID n=0; n < num_nodes; n++)
    biggest_score = std::max(biggest_score, scores[n]);
  #pragma omp parallel for
  for (NodeID n=0; n < num_nodes; n++)
    scores[n] = scores[n] / biggest_score;
  return scores;
}

// int main(int argc, char* argv[]) {
//   CLIterApp cli(argc, argv, "betweenness-centrality", 1);
//   if (!cli.ParseArgs())
//     return -1;
//   if (cli.num_iters() > 1 && cli.start_vertex() != -1)
//     cout << "Warning: iterating from same source (-r & -i)" << endl;
//   Builder b(cli);
//   Graph g = b.MakeGraph();
//   SourcePicker<Graph> sp(g, cli.start_vertex());
//   auto BCBound = [&sp, &cli] (const Graph &g) {
//     return Brandes(g, sp, cli.num_iters(), cli.logging_en());
//   };
//   SourcePicker<Graph> vsp(g, cli.start_vertex());
//   auto VerifierBound = [&vsp, &cli] (const Graph &g,
//                                      const pvector<ScoreT> &scores) {
//     return BCVerifier(g, vsp, cli.num_iters(), scores);
//   };
//   BenchmarkKernel(cli, g, BCBound, PrintTopScores, VerifierBound);
//   return 0;
// }