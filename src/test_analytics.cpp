/*
 * Copyright (C) 2025 Haoxuan Xie
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "utils.h"
#include "radixgraph.h"
#include "optimizer.h"
#include "GAPBS/bfs.h"
#include "GAPBS/sssp.h"
#include "GAPBS/pr_spmv.h"
#include "GAPBS/cc_sv.h"
#include "GAPBS/bc.h"
#include "GAPBS/tc.h"
#define EVALUATE_ONE_HOP 1
#define EVALUATE_TWO_HOP 1
#define EVALUATE_BFS 1
#define EVALUATE_SSSP 1
#define EVALUATE_PR 1
#define EVALUATE_WCC 1
#define EVALUATE_LCC 1
#define EVALUATE_BC 1

int main(int argc, char* argv[]) {
    #if USE_SORT
        if (argc < 2) {
            std::cout << "Usage: ./test_edge_chain <edge_file>" << std::endl;
            return 0;
        }
        // Read edge from the file
        std::ifstream fin(argv[1]);
        if (!fin.is_open()) {
            std::cout << "Error: cannot open file " << argv[1] << std::endl;
            return 0;
        }
        std::string line;
        std::vector<std::pair<uint32_t, uint32_t>> edges;
        std::unordered_set<int> vertex_set;
        uint32_t max_id = 0, source = 0;
        bool has_source = false;
        while (std::getline(fin, line)) {
            if (line.empty() || line[0] == '#') continue;
            std::istringstream iss(line);
            uint32_t u, v;
            if (!(iss >> u >> v)) { break; } // error
            edges.emplace_back(u, v);
            max_id = std::max(max_id, std::max(u, v));
            vertex_set.insert(u);
            vertex_set.insert(v);
            if (!has_source) source = u, has_source = true;
        }
        int l = 5, n = vertex_set.size();
        auto a = OptimizeParameters(n, 32, l);
        RadixGraph g(l, a);
        #pragma omp parallel for
        for (auto e : edges) {
            g.InsertEdge(e.first, e.second, 1.0);
        }
        std::cout << "Graph loaded. Number of vertices: " << vertex_set.size() << ", number of edges: " << edges.size() << std::endl;
        std::chrono::high_resolution_clock::time_point start, end;
        std::chrono::duration<double> duration;
        // Sample some vertices for hop-query
        std::default_random_engine generator(42);
        int sample_num = std::min(1000, (int)vertex_set.size());
        std::vector<int> vertex_vec(vertex_set.begin(), vertex_set.end());
        std::shuffle(vertex_vec.begin(), vertex_vec.end(), generator);
        #if EVALUATE_ONE_HOP
            // Test efficiency of 1-hop neighbors
            start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < sample_num; i++) {
                std::vector<WeightedEdge> neighbours;
                g.GetNeighbours(vertex_vec[i], neighbours);
            }
            end = std::chrono::high_resolution_clock::now();
            duration = end - start;
            std::cout << "1-hop neighbors: time = " << duration.count() << "s" << std::endl;
        #endif
        #if EVALUATE_TWO_HOP
            // Test efficiency of 2-hop neighbors
            start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < sample_num; i++) {
                std::vector<WeightedEdge> neighbours;
                g.GetNeighbours(vertex_vec[i], neighbours);
                for (auto e : neighbours) {
                    std::vector<WeightedEdge> neighbours2;
                    #if USE_EDGE_CHAIN
                      g.GetNeighboursByOffset(e.idx, neighbours2);
                    #else
                      g.GetNeighbours(g->vertex_index->vertex_table[e.idx].node, neighbours2);
                    #endif
                }
            }
            end = std::chrono::high_resolution_clock::now();
            duration = end - start;
            std::cout << "2-hop neighbors: time = " << duration.count() << "s" << std::endl;
        #endif
        #if EVALUATE_BFS
            // Test efficiency of BFS
            start = std::chrono::high_resolution_clock::now();
            DOBFS(&g, source, n, edges.size(), -1);
            end = std::chrono::high_resolution_clock::now();
            duration = end - start;
            std::cout << "BFS: time = " << duration.count() << "s" << std::endl;
        #endif
        #if EVALUATE_SSSP
            // Test efficiency of SSSP
            start = std::chrono::high_resolution_clock::now();
            DeltaStep(&g, source, 2.0, n, edges.size());
            end = std::chrono::high_resolution_clock::now();
            duration = end - start;
            std::cout << "SSSP: time = " << duration.count() << "s" << std::endl;
        #endif
        #if EVALUATE_PR
            // Test efficiency of PageRank
            start = std::chrono::high_resolution_clock::now();
            PageRankPull(&g, 10, max_id + 1);
            end = std::chrono::high_resolution_clock::now();
            duration = end - start;
            std::cout << "PageRank: time = " << duration.count() << "s" << std::endl;
        #endif
        #if EVALUATE_WCC
            // Test efficiency of WCC
            start = std::chrono::high_resolution_clock::now();
            ShiloachVishkin(&g, max_id + 1);
            end = std::chrono::high_resolution_clock::now();
            duration = end - start;
            std::cout << "WCC: time = " << duration.count() << "s" << std::endl;
        #endif
        #if EVALUATE_LCC
            // Test efficiency of LCC
            start = std::chrono::high_resolution_clock::now();
            OrderedCount(&g, max_id + 1);
            end = std::chrono::high_resolution_clock::now();
            duration = end - start;
            std::cout << "LCC: time = " << duration.count() << "s" << std::endl;
        #endif
        #if EVALUATE_BC
            // Test efficiency of BC
            start = std::chrono::high_resolution_clock::now();
            Brandes(&g, 5, max_id + 1);
            end = std::chrono::high_resolution_clock::now();
            duration = end - start;
            std::cout << "BC: time = " << duration.count() << "s" << std::endl;
        #endif
    #else
        std::cout << "Please enable USE_SORT to test analytics." << std::endl;
    #endif
    return 0;

}