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


using namespace std;

vector<int> BubbleSort(vector<int> data)
{
    // 拷贝
    vector<int> result = data;
    // 排序过程：最多扫描n-1次，n为数据个数
    int size = static_cast<int>(data.size());
    for (int t = size - 1; t >= 0; --t)
    {
        // 若已排序好，扫描一遍后，flag应为false，此时可中断以达到提速效果
        bool flag = false;
        for (int i = 0; i < t; ++i)
        for (int i = 0; i < t; ++i)
        {
            if (result[i] > result[i + 1])
            {
                int temp = result[i];
                result[i] = result[i + 1];
                result[i + 1] = temp;
                flag = true;
            }
        }
        if (!flag)
            break;
    }
    return result;
}

vector<double> OrderedCount(SpruceTransVer::TopBlock* g, uint32_t num_vertices) {
  size_t total = 0;
  auto triangles_per_vertex = (std::atomic<uint32_t>*)malloc(sizeof(std::atomic<uint32_t>) * num_vertices);
//  std::atomic<uint32_t> triangles_per_vertex[num_vertices];
#pragma omp parallel
  {
    auto triangles_u = 0u;
    auto triangles_v = 0u;

#pragma omp for /*reduction(+ : total) */schedule(dynamic, 256)
    for (NodeID u = 0; u < num_vertices/*g.num_nodes()*/; u++) {
        std::vector<uint32_t> u_neighbours;
        SpruceTransVer::get_neighbours_sorted_only(g, u, u_neighbours);
//        std::sort(u_neighbours.begin(), u_neighbours.end());
        for (auto v: /*g.out_neigh(u)*/u_neighbours) {
            if (v > u) {
                break;
            }
            auto it = /*g.out_neigh(u)*/u_neighbours.begin();
            std::vector<uint32_t> v_neighbours;
            SpruceTransVer::get_neighbours_sorted_only(g, v, v_neighbours);
//            std::sort(v_neighbours.begin(), v_neighbours.end());
            for (auto w: /*g.out_neigh(v)*/v_neighbours) {
                if (w > v) {
                    break;
                }
                while ((*it) < w) {
                    it++;
                }
                if (w == (*it)) {
//                    total++;
                    triangles_u += 2;
                    triangles_v += 2;
                    triangles_per_vertex[w] += 2;
                }
            }
            triangles_per_vertex[v]+= triangles_v;
            triangles_v = 0;
        }
        triangles_per_vertex[u] += triangles_u;
        triangles_u = 0;
    }
}
    vector<double> lcc_values(num_vertices);
#pragma omp parallel for
    for (NodeID v = 0; v < num_vertices; v++) {
        uint32_t degree = SpruceTransVer::GetDegree(g, v);
        uint64_t max_num_edges = degree * (degree - 1);
        if (max_num_edges != 0) {
            lcc_values[v] = ((double) triangles_per_vertex[v]) / max_num_edges;
        } else {
            lcc_values[v] = 0.0;
        }
    }
  return lcc_values;
}


// heuristic to see if sufficently dense power-law graph
bool WorthRelabelling(const Graph &g) {
  int64_t average_degree = g.num_edges() / g.num_nodes();
  if (average_degree < 10)
    return false;
  SourcePicker<Graph> sp(g);
  int64_t num_samples = min(int64_t(1000), g.num_nodes());
  int64_t sample_total = 0;
  pvector<int64_t> samples(num_samples);
  for (int64_t trial=0; trial < num_samples; trial++) {
    samples[trial] = g.out_degree(sp.PickNext());
    sample_total += samples[trial];
  }
  sort(samples.begin(), samples.end());
  double sample_average = static_cast<double>(sample_total) / num_samples;
  double sample_median = samples[num_samples/2];
  return sample_average / 1.3 > sample_median;
}


// uses heuristic to see if worth relabeling
size_t Hybrid(const Graph &g) {
//  if (WorthRelabelling(g))
//    return OrderedCount(Builder::RelabelByDegree(g));
//  else
//    return OrderedCount(g);
}


void PrintTriangleStats(const Graph &g, size_t total_triangles) {
  cout << total_triangles << " triangles" << endl;
}


// Compares with simple serial implementation that uses std::set_intersection
bool TCVerifier(const Graph &g, size_t test_total) {
  size_t total = 0;
  vector<NodeID> intersection;
  intersection.reserve(g.num_nodes());
  for (NodeID u : g.vertices()) {
    for (NodeID v : g.out_neigh(u)) {
      auto new_end = set_intersection(g.out_neigh(u).begin(),
                                      g.out_neigh(u).end(),
                                      g.out_neigh(v).begin(),
                                      g.out_neigh(v).end(),
                                      intersection.begin());
      intersection.resize(new_end - intersection.begin());
      total += intersection.size();
    }
  }
  total = total / 6;  // each triangle was counted 6 times
  if (total != test_total)
    cout << total << " != " << test_total << endl;
  return total == test_total;
}


//int main(int argc, char* argv[]) {
//  CLApp cli(argc, argv, "triangle count");
//  if (!cli.ParseArgs())
//    return -1;
//  Builder b(cli);
//  Graph g = b.MakeGraph();
//  if (g.directed()) {
//    cout << "Input graph is directed but tc requires undirected" << endl;
//    return -2;
//  }
//  BenchmarkKernel(cli, g, Hybrid, PrintTriangleStats, TCVerifier);
//  return 0;
//}
