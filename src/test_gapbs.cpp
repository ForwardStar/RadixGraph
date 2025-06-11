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
#include "headers.h"
#include "radixgraph.h"
#include "./GAPBS/bfs.h"
#include "./GAPBS/sssp.h"
#include "./GAPBS/tc.h"
#include "./GAPBS/cc_sv.h"
#include "./GAPBS/pr_spmv.h"

int main(int argc, char* argv[]) {
    std::ios::sync_with_stdio(false);
    srand((int)time(NULL));
    if (argc > 1) {
        std::ifstream f("settings");
        int d;
        f >> d;
        std::vector<int> a(d);
        for (auto& i : a) f >> i;

        std::ifstream fin(argv[argc - 1]);
        uint64_t u, v, s;
        bool has_source = false;
        std::vector<std::pair<uint64_t, uint64_t>> edges;
        int m = 0;
        while (fin >> u >> v) {
            edges.emplace_back(u, v);
            if (!has_source) s = u, has_source = true;
            m++;
        }

        RadixGraph G(d, a);
        #pragma omp parallel for
        for (auto e : edges) G.InsertEdge(e.first, e.second, 0.5);
        int n = G.vertex_index->cnt;

        std::cout << "Testing BFS..." << std::endl;
        auto start = std::chrono::high_resolution_clock::now();
        auto p = DOBFS(&G, s, n, m, -1);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        std::cout << "Time: " << duration.count() << "s" << std::endl;

        // Test SSSP
        std::cout << "Testing SSSP..." << std::endl;
        start = std::chrono::high_resolution_clock::now();
        auto res4 = DeltaStep(&G, s, 2.0, n, m);
        end = std::chrono::high_resolution_clock::now();
        duration = end - start;
        std::cout << "Time: " << duration.count() << "s" << std::endl;

        // Test WCC
        std::cout << "Testing WCC..." << std::endl;
        start = std::chrono::high_resolution_clock::now();
        ShiloachVishkin(&G, n);
        end = std::chrono::high_resolution_clock::now();
        duration = end - start;
        std::cout << "Time: " << duration.count() << "s" << std::endl;

        // Test PageRank
        std::cout << "Testing PageRank..." << std::endl;
        start = std::chrono::high_resolution_clock::now();
        PageRankPull(&G, 10, n);
        end = std::chrono::high_resolution_clock::now();
        duration = end - start;
        std::cout << "Time: " << duration.count() << "s" << std::endl;

        return 0;
    }

    int d = 3;
    std::vector<int> a = {7, 7, 6};
    int n = 100000, m = 10000000;
    int num_trials = 3;
    
    std::default_random_engine generator;
    unsigned long long maximum = (1ull << 20) - 1;
    std::uniform_int_distribution distribution(0ull, maximum);

    std::vector<uint64_t> vertex_ids;
    std::unordered_set<uint64_t> vertex_set;
    for (int i = 0; i < n; i++) {
        uint64_t id = distribution(generator);
        while (vertex_set.find(id) != vertex_set.end()) {
            id = distribution(generator);
        }
        vertex_ids.push_back(id);
        vertex_set.insert(id);
    }

    std::vector<std::pair<std::pair<uint64_t, uint64_t>, double>> edges;
    uint64_t u, v;
    double w;
    // for (int i = 0; i < n - 1; i++) {
    //     edges.push_back(std::make_pair(std::make_pair(vertex_ids[i], vertex_ids[i + 1]), 0.5));
    // }
    std::set<std::pair<uint64_t, uint64_t>> edge_set;
    for (int i = 0; i < m; i++) {
        int id1 = rand() % n, id2 = rand() % n;
        u = vertex_ids[id1];
        v = vertex_ids[id2];
        while (edge_set.find({u, v}) != edge_set.end()) {
            id1 = rand() % n, id2 = rand() % n;
            u = vertex_ids[id1];
            v = vertex_ids[id2];
        }
        edges.push_back(std::make_pair(std::make_pair(u, v), 0.5));
        edge_set.emplace(std::make_pair(u, v));
    }

    RadixGraph G(d, a);
    #pragma omp parallel for
    for (auto e : edges) {
        G.InsertEdge(e.first.first, e.first.second, e.second);
    }

    // Test BFS
    std::cout << "Testing BFS..." << std::endl;
    auto res1 = G.BFS(vertex_ids[0]);
    auto p = DOBFS(&G, vertex_ids[0], n, m, -1);
    std::vector<uint64_t> res2;
    for (int i = 0; i < n; i++) {
        if (p[i] != -1) {
            res2.emplace_back(G.vertex_index->vertex_table[i].node);
        }
    }
    std::sort(res1.begin(), res1.end());
    std::sort(res2.begin(), res2.end());
    if (res1.size() != res2.size()) {
        std::cout << "BFS wrong results detected. Expected size = " << res1.size() << ", actual size = " << res2.size() << std::endl;
        return 0;
    }
    for (int i = 0; i < res1.size(); i++) {
        if (res1[i] != res2[i]) {
            std::cout << "BFS wrong results detected. Wrong node id." << std::endl;
            return 0;
        }
    }
    std::cout << "BFS results verified!" << std::endl;

    // Test SSSP
    std::cout << "Testing SSSP..." << std::endl;
    auto res3 = G.SSSP(vertex_ids[0]);
    auto res4 = DeltaStep(&G, vertex_ids[0], 2.0, n, m);
    if (res3.size() != res4.size()) {
        std::cout << "SSSP wrong results detected. Expected size = " << res3.size() << ", actual size = " << res4.size() << std::endl;
        return 0;
    }
    for (int i = 0; i < res3.size(); i++) {
        if ((res3[i] <= 1e9 || res4[i] <= 1e9) && abs(res3[i] - res4[i]) > 1e-6) {
            std::cout << "SSSP wrong results detected. Distance of node " << G.vertex_index->vertex_table[i].node << " is expected to be: " << res3[i] << ", actual: " << res4[i] << std::endl;
            return 0;
        }
    }
    std::cout << "SSSP results verified!" << std::endl;

    // Test LCC
    std::cout << "Testing LCC..." << std::endl;
    OrderedCount(&G, n);

    // Test WCC
    std::cout << "Testing WCC..." << std::endl;
    ShiloachVishkin(&G, n);

    // Test PageRank
    std::cout << "Testing PageRank..." << std::endl;
    PageRankPull(&G, 100, n);

    return 0;
}