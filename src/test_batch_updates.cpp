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
#include "radixgraph.h"
#define NUM_THREADS 64

int main(int argc, char* argv[]) {
    std::ios::sync_with_stdio(false);
    srand((int)time(NULL));
    if (argc <= 1) {
        std::cout << "Usage: ./test_batch_updates [input_file]" << std::endl;
        return 0;
    }

    if (argc > 1) {
        // There is input file, read edges from file
        // Format: each line contains "src des" or "src des weight"
        std::ifstream fin(argv[1]);
        uint64_t src, des;
        float weight = 1.0;
        std::vector<std::pair<std::pair<uint64_t, uint64_t>, float>> edges;
        while (fin >> src >> des) {
            if (fin.peek() == ' ' || fin.peek() == '\t') {
                fin >> weight;
            }
            edges.emplace_back(std::make_pair(std::make_pair(src, des), weight));
            weight = 1.0;  // Reset weight for the next edge
        }
        std::shuffle(edges.begin(), edges.end(), std::default_random_engine(0));
        // Read SORT configuration from "settings" file
        std::ifstream fin_settings("settings");
        int d;
        fin_settings >> d;
        std::vector<int> a(d);
        for (auto& i : a) fin_settings >> i;

        #if USE_SORT
            RadixGraph G(d, a, NUM_THREADS);
        #elif USE_ART
            RadixGraph G(NUM_THREADS);
        #else
            RadixGraph G(NUM_THREADS);
        #endif
        // Insert all edges
        std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
        auto start_memory = get_proc_mem(); // record memory
        start = std::chrono::high_resolution_clock::now();
        #pragma omp parallel for
        for (auto e : edges) G.InsertEdge(e.first.first, e.first.second, e.second), G.InsertEdge(e.first.second, e.first.first, e.second);
        end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        auto end_memory = get_proc_mem(); // record memory
        std::cout << "Initial graph loaded in " << duration.count() << "s" << std::endl;
        std::cout << "Throughput: " << edges.size() * 2 / duration.count() << " ops/s" << std::endl;
        std::cout << "Memory usage: " << (end_memory - start_memory) / 1024.0 / 1024.0 << " GB" << std::endl;

        // Generate batch updates
        std::vector<uint32_t> update_sizes = {10000, 100000, 1000000, 10000000};
        for (auto update_size : update_sizes) {
            double avg_insert = 0;
            double avg_delete = 0;
            int n_trials = 1;
            for (size_t ts = 0; ts < n_trials; ts++) {
                int n = G.vertex_index->cnt;
                std::vector<uint32_t> new_srcs(update_size);
                std::vector<uint32_t> new_dests(update_size);
                #pragma omp parallel for
                for (uint32_t i = 0; i < update_size; i++) {
                    new_srcs[i] = rand() % n;
                    new_dests[i] = rand() % n;
                }
                // Test batch insert
                std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
                start = std::chrono::high_resolution_clock::now();
                #pragma omp parallel for num_threads(NUM_THREADS)
                for (uint32_t i = 0; i < update_size; i++) {
                    G.InsertEdge(new_srcs[i], new_dests[i], 1.0);
                }
                end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> duration = end - start;
                avg_insert += duration.count();
                // Test batch delete
                start = std::chrono::high_resolution_clock::now();
                #pragma omp parallel for num_threads(NUM_THREADS)
                for (uint32_t i = 0; i < update_size; i++) {
                    G.DeleteEdge(new_srcs[i], new_dests[i]);
                }
                end = std::chrono::high_resolution_clock::now();
                duration = end - start;
                avg_delete += duration.count();
            }
            avg_insert /= n_trials;
            avg_delete /= n_trials;
            std::cout << "Batch size: " << update_size << ", average insert time: " << avg_insert << "s, average delete time: " << avg_delete << "s" << std::endl;
            std::cout << "Batch size: " << update_size << ", average insert throughput: " << update_size / avg_insert << " ops/s, average delete throughput: " << update_size / avg_delete << " ops/s" << std::endl;
        }
    }
}