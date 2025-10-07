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

int main(int argc, char* argv[]) {
    std::ios::sync_with_stdio(false);
    srand((int)time(NULL));

    std::vector<int> d = {3, 3, 3};
    std::vector<std::vector<int>> a = {
        {19, 6, 5},
        {21, 5, 4},
        {23, 4, 3},
    };
    int m = 10000000;
    int num_trials = 3;
    int num_threads = 64;
    
    std::default_random_engine generator;
    unsigned long long maximum = (1ull << 30) - 1;
    std::uniform_int_distribution distribution(0ull, maximum);
    int now = 0;
    for (int n = 10000; n <= 1000000; n *= 10) {
        double duration_insert_edge_fstar = 0;
        double duration_delete_edge_fstar = 0;
        double duration_update_edge_fstar = 0;
        double duration_get_neighbours_fstar = 0;
        double duration_bfs_fstar = 0;
        
        std::cout << "n = " << n << ", m = " << m << std::endl;
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
        for (int i = 0; i < num_trials; i++) {
            // std::cout << "Trial " << i + 1 << ":" << std::endl;
            RadixGraph G_fstar(d[now], a[now]);
            G_fstar.Init(num_threads, n);
            std::vector<std::pair<std::pair<uint64_t, uint64_t>, double>> edges;

            // Insert edges
            {
                uint64_t u, v;
                double w;
                std::set<std::pair<uint64_t, uint64_t>> edge_set;
                for (int i = 0; i < n - 1; i++) {
                    edges.push_back(std::make_pair(std::make_pair(vertex_ids[i], vertex_ids[i + 1]), 0.5));
                    edge_set.emplace(std::make_pair(vertex_ids[i], vertex_ids[i + 1]));
                }
                for (int i = 0; i < m - (n - 1); i++) {
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

                auto start = std::chrono::high_resolution_clock::now();
                #if USE_SORT
                    #pragma omp parallel for num_threads(num_threads)
                    for (auto e : edges) {
                        G_fstar.InsertEdge(e.first.first, e.first.second, e.second);
                    }
                #elif USE_ART
                    unodb::this_thread().qsbr_pause();
                    std::vector<unodb::qsbr_thread> threads(num_threads);
                    for (int i = 0; i < num_threads; i++) {
                        threads[i] = unodb::qsbr_thread([&, i]() {
                            G_fstar.thread_id_local = i;
                            for (int j = i; j < edges.size(); j += num_threads) {
                                G_fstar.InsertEdge(edges[j].first.first, edges[j].first.second, edges[j].second);
                            }
                        });
                    }
                    for (int i = 0; i < num_threads; i++) {
                        threads[i].join();
                    }
                    unodb::this_thread().qsbr_resume();
                    unodb::this_thread().quiescent();
                    unodb::this_thread().quiescent();
                #endif
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> duration = end - start;
                duration_insert_edge_fstar += duration.count();
            }

            // std::cout << "Insert done" << std::endl;

            // Update edges
            {         
                std::vector<std::pair<std::pair<uint64_t, uint64_t>, double>> edges_update;
                for (int i = 0; i < 10000; i++) {
                    auto e = edges[rand() % m];
                    edges_update.push_back(e);
                }

                auto start = std::chrono::high_resolution_clock::now();
                #if USE_SORT
                    #pragma omp parallel for num_threads(num_threads)
                    for (auto e : edges_update) {
                        G_fstar.UpdateEdge(e.first.first, e.first.second, e.second);
                    }
                #elif USE_ART
                    unodb::this_thread().qsbr_pause();
                    std::vector<unodb::qsbr_thread> threads(num_threads);
                    for (int i = 0; i < num_threads; i++) {
                        threads[i] = unodb::qsbr_thread([&, i]() {
                            G_fstar.thread_id_local = i;
                            for (int j = i; j < edges_update.size(); j += num_threads) {
                                G_fstar.UpdateEdge(edges_update[j].first.first, edges_update[j].first.second, edges_update[j].second);
                            }
                        });
                    }
                    for (int i = 0; i < num_threads; i++) {
                        threads[i].join();
                    }
                    unodb::this_thread().qsbr_resume();
                    unodb::this_thread().quiescent();
                    unodb::this_thread().quiescent();
                #endif
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> duration = end - start;
                duration_update_edge_fstar += duration.count();
            }

            // std::cout << "Update done" << std::endl;

            // Delete edges
            {         
                std::vector<std::pair<std::pair<uint64_t, uint64_t>, double>> edges_delete;
                std::unordered_set<int> idx_delete;
                for (int i = 0; i < 10000; i++) {
                    auto id = rand() % m;
                    while (idx_delete.find(id) != idx_delete.end() || id < n - 1) {
                        id = rand() % m;
                    }
                    idx_delete.insert(id);
                    auto e = edges[id];
                    edges_delete.push_back(e);
                }

                auto start = std::chrono::high_resolution_clock::now();
                #if USE_SORT
                    #pragma omp parallel for num_threads(num_threads)
                    for (auto e : edges_delete) {
                        G_fstar.DeleteEdge(e.first.first, e.first.second);
                    }
                #elif USE_ART
                    unodb::this_thread().qsbr_pause();
                    std::vector<unodb::qsbr_thread> threads(num_threads);
                    for (int i = 0; i < num_threads; i++) {
                        threads[i] = unodb::qsbr_thread([&, i]() {
                            G_fstar.thread_id_local = i;
                            for (int j = i; j < edges_delete.size(); j += num_threads) {
                                G_fstar.DeleteEdge(edges_delete[j].first.first, edges_delete[j].first.second);
                            }
                        });
                    }
                    for (int i = 0; i < num_threads; i++) {
                        threads[i].join();
                    }
                    unodb::this_thread().qsbr_resume();
                    unodb::this_thread().quiescent();
                    unodb::this_thread().quiescent();
                #endif
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> duration = end - start;
                duration_delete_edge_fstar += duration.count();
            }

            // std::cout << "Delete done" << std::endl;

            // Get neighbours
            {
                #if USE_SORT
                    G_fstar.CreateSnapshots();
                #elif USE_ART
                    unodb::this_thread().qsbr_pause();
                    unodb::qsbr_thread build_thread = unodb::qsbr_thread([&]() {
                        G_fstar.CreateSnapshots();
                    });
                    build_thread.join();
                    unodb::this_thread().qsbr_resume();
                    unodb::this_thread().quiescent();
                    unodb::this_thread().quiescent();
                #endif
                auto start = std::chrono::high_resolution_clock::now();
                #if USE_SORT
                    #pragma omp parallel for num_threads(num_threads)
                    for (int j = 0; j < n; j++) {
                        std::vector<WeightedEdge> neighbours;
                        G_fstar.GetNeighbours(vertex_ids[j], neighbours);
                    }
                #elif USE_ART
                    unodb::this_thread().qsbr_pause();
                    std::vector<unodb::qsbr_thread> threads(num_threads);
                    for (int i = 0; i < num_threads; i++) {
                        threads[i] = unodb::qsbr_thread([&, i]() {
                            G_fstar.thread_id_local = i;
                            for (int j = i; j < n; j += num_threads) {
                                std::vector<WeightedEdge> neighbours;
                                G_fstar.GetNeighbours(vertex_ids[j], neighbours);
                            }
                        });
                    }
                    for (int i = 0; i < num_threads; i++) {
                        threads[i].join();
                    }
                    unodb::this_thread().qsbr_resume();
                    unodb::this_thread().quiescent();
                    unodb::this_thread().quiescent();
                #endif
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> duration = end - start;
                duration_get_neighbours_fstar += duration.count();
            }

            // std::cout << "Get neighbors done" << std::endl;

            // BFS
            {
                auto start = std::chrono::high_resolution_clock::now();
                #if USE_SORT
                    auto res_fstar = G_fstar.BFS(vertex_ids[0]);
                #elif USE_ART
                    unodb::this_thread().qsbr_pause();
                    unodb::this_thread().qsbr_pause();
                    unodb::qsbr_thread bfs_thread = unodb::qsbr_thread([&]() {
                        G_fstar.thread_id_local = 0;
                        auto res_fstar = G_fstar.BFS(vertex_ids[0]);
                    });
                    bfs_thread.join();
                    unodb::this_thread().qsbr_resume();
                    unodb::this_thread().quiescent();
                    unodb::this_thread().quiescent();
                    unodb::this_thread().qsbr_resume();
                    unodb::this_thread().quiescent();
                    unodb::this_thread().quiescent();
                #endif
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> duration = end - start;
                duration_bfs_fstar += duration.count();
            }

            // std::cout << "BFS done" << std::endl;
        }
        ++now;

        std::cout << "Average insertion time for RadixGraph: " << duration_insert_edge_fstar / num_trials << "s" << std::endl;
        std::cout << "Average deletion time for RadixGraph: " << duration_delete_edge_fstar / num_trials << "s" << std::endl;
        std::cout << "Average update time for RadixGraph: " << duration_update_edge_fstar / num_trials << "s" << std::endl;
        std::cout << "Average get neighbours time for RadixGraph: " << duration_get_neighbours_fstar / num_trials << "s" << std::endl;
        std::cout << "Average BFS time for RadixGraph: " << duration_bfs_fstar / num_trials << "s" << std::endl;
    }
}