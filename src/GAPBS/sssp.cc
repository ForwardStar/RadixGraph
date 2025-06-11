// Copyright (c) 2015, The Regents of the University of California (Regents)
// See LICENSE.txt for license details

#include "sssp.h"

/*
GAP Benchmark Suite
Kernel: Single-source Shortest Paths (SSSP)
Author: Scott Beamer, Yunming Zhang

Returns array of distances for all vertices from given source vertex

This SSSP implementation makes use of the ∆-stepping algorithm [1]. The type
used for weights and distances (WeightT) is typedefined in benchmark.h. The
delta parameter (-d) should be set for each input graph. This implementation
incorporates a new bucket fusion optimization [2] that significantly reduces
the number of iterations (& barriers) needed.

The bins of width delta are actually all thread-local and of type std::vector
so they can grow but are otherwise capacity-proportional. Each iteration is
done in two phases separated by barriers. In the first phase, the current
shared bin is processed by all threads. As they find vertices whose distance
they are able to improve, they add them to their thread-local bins. During this
phase, each thread also votes on what the next bin should be (smallest
non-empty bin). In the next phase, each thread copies its selected
thread-local bin into the shared bin.

Once a vertex is added to a bin, it is not removed, even if its distance is
later updated and it now appears in a lower bin. We find ignoring vertices if
their distance is less than the min distance for the current bin removes
enough redundant work to be faster than removing the vertex from older bins.

The bucket fusion optimization [2] executes the next thread-local bin in
the same iteration if the vertices in the next thread-local bin have the
same priority as those in the current shared bin. This optimization greatly
reduces the number of iterations needed without violating the priority-based
execution order, leading to significant speedup on large diameter road networks.

[1] Ulrich Meyer and Peter Sanders. "δ-stepping: a parallelizable shortest path
    algorithm." Journal of Algorithms, 49(1):114–152, 2003.

[2] Yunming Zhang, Ajay Brahmakshatriya, Xinyi Chen, Laxman Dhulipala,
    Shoaib Kamil, Saman Amarasinghe, and Julian Shun. "Optimizing ordered graph
    algorithms with GraphIt." The 18th International Symposium on Code Generation
    and Optimization (CGO), pages 158-170, 2020.
*/
const WeightT kDistInf = std::numeric_limits<WeightT>::max() / 2;
const size_t kMaxBin = std::numeric_limits<size_t>::max() / 2;
const size_t kBinSizeThreshold = 1000;


pvector<WeightT> DeltaStep(ForwardStar* g, NodeID source, WeightT delta, uint32_t num_nodes, long num_edges) {
  // default delta is 2.0
  pvector<WeightT> dist(num_nodes, kDistInf);
  auto u = g->vertex_index->RetrieveVertex(source);
  int uidx = u->idx;
  dist[uidx] = 0;
  pvector<int> frontier(num_edges);
  // two element arrays for double buffering curr=iter&1, next=(iter+1)&1
  size_t shared_indexes[2] = {0, kMaxBin};
  size_t frontier_tails[2] = {1, 0};
  frontier[0] = uidx;
  #pragma omp parallel
  {
    std::vector<std::vector<int> > local_bins(0);
    size_t iter = 0;
    while (shared_indexes[iter&1] != kMaxBin) {
      size_t &curr_bin_index = shared_indexes[iter&1];
      size_t &next_bin_index = shared_indexes[(iter+1)&1];
      size_t &curr_frontier_tail = frontier_tails[iter&1];
      size_t &next_frontier_tail = frontier_tails[(iter+1)&1];
      #pragma omp for nowait schedule(dynamic, 64)
      for (size_t i=0; i < curr_frontier_tail; i++) {
        auto u = frontier[i];
        if (dist[u] >= delta * static_cast<WeightT>(curr_bin_index))
          RelaxEdges(g, u, delta, dist, local_bins);
      }
      while (curr_bin_index < local_bins.size() &&
             !local_bins[curr_bin_index].empty() &&
             local_bins[curr_bin_index].size() < kBinSizeThreshold) {
        std::vector<int> curr_bin_copy = local_bins[curr_bin_index];
        local_bins[curr_bin_index].resize(0);
        for (auto u : curr_bin_copy)
          RelaxEdges(g, u, delta, dist, local_bins);
      }
      for (size_t i=curr_bin_index; i < local_bins.size(); i++) {
        if (!local_bins[i].empty()) {
          #pragma omp critical
          next_bin_index = std::min(next_bin_index, i);
          break;
        }
      }
      #pragma omp barrier
      #pragma omp single nowait
      {
        curr_bin_index = kMaxBin;
        curr_frontier_tail = 0;
      }
      if (next_bin_index < local_bins.size()) {
        size_t copy_start = fetch_and_add(next_frontier_tail,
                                          local_bins[next_bin_index].size());
        copy(local_bins[next_bin_index].begin(),
             local_bins[next_bin_index].end(), frontier.data() + copy_start);
        local_bins[next_bin_index].resize(0);
      }
      iter++;
      #pragma omp barrier
    }
  }
  return dist;
}