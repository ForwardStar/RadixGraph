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
#include "../Spruce/Spruce/src/spruce_transaction.h"

uint64_t max_uid = 0;

std::vector<uint64_t> spruce_bfs(SpruceTransVer& spruce, uint64_t src) {
    AtomicBitmap vis(max_uid + 1);
    vis.reset();
    std::queue<uint64_t> Q;
    vis.set_bit(src);
    Q.push(src);
    std::vector<uint64_t> res;
    while (!Q.empty()) {
        auto u = Q.front();
        Q.pop();
        res.push_back(u);
        std::vector<SpruceTransVer::WeightedOutEdgeSimple> neighbours;
        SpruceTransVer::get_neighbours(spruce, u, neighbours);
        for (auto e : neighbours) {
            if (!vis.get_bit(e.des)) {
                vis.set_bit(e.des);
                Q.push(e.des);
            }
        }
    }
    return res;
}

unsigned int get_proc_mem() {
    int pid = getpid();
    char file_name[64] = { 0 };
    FILE* fd;
    char line_buff[512] = { 0 };
    sprintf(file_name, "/proc/%d/status", pid);
    fd = fopen(file_name, "r");
    if (fd == NULL) return 0;
    char name[64];
    int vmrss = 0;
    int i;
    for (i = 0; i < VMRSS_LINE - 1; i++) {
        fgets(line_buff, sizeof(line_buff), fd);
    }
    fgets(line_buff, sizeof(line_buff), fd);
    sscanf(line_buff, "%s %d", name, &vmrss);
    fclose(fd);
    return vmrss;
}

int main(int argc, char* argv[]) {
    std::ios::sync_with_stdio(false);
    srand((int)time(NULL));
    int num_threads = 16;
    if (argc > 1) {
        std::ifstream f("settings");
        int d;
        f >> d;
        std::vector<int> a(d);
        for (auto& i : a) f >> i;

        std::ifstream fin(argv[argc - 1]);
        uint64_t u, v;
        std::vector<std::pair<uint64_t, uint64_t>> edges;
        while (fin >> u >> v) {
            edges.emplace_back(u, v);
            max_uid = std::max(max_uid, u);
            max_uid = std::max(max_uid, v);
        }
        std::random_shuffle(edges.begin(), edges.end());

        int start_mem = get_proc_mem();
        RadixGraph G_fstar(d, a);
        auto start = std::chrono::high_resolution_clock::now();
        #pragma omp parallel for num_threads(num_threads)
        for (auto e : edges) G_fstar.InsertEdge(e.first, e.second, 0.5);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        std::cout << "Forward*: " << duration.count() << "s" << std::endl;
        std::cout << "Size: " << G_fstar.vertex_index->size() << std::endl;
        std::cout << "Memory: " << get_proc_mem() - start_mem << std::endl;

        start_mem = get_proc_mem();
        SpruceTransVer spruce;
        start = std::chrono::high_resolution_clock::now();
        #pragma omp parallel for num_threads(num_threads)
        for (auto e : edges) SpruceTransVer::InsertEdge(spruce, {e.first, e.second, 0.5});
        end = std::chrono::high_resolution_clock::now();
        duration = end - start;
        std::cout << "Spruce: " << duration.count() << "s" << std::endl;
        std::cout << "Memory: " << get_proc_mem() - start_mem << std::endl;

        return 0;
    }
    std::vector<int> d = {3, 3, 3};
    std::vector<std::vector<int>> a = {
        {19, 6, 5},
        {21, 5, 4},
        {23, 4, 3},
    };
    int m = 10000000;
    int num_trials = 3;
    
    std::default_random_engine generator;
    unsigned long long maximum = (1ull << 30) - 1;
    std::uniform_int_distribution distribution(0ull, maximum);
    int now = 0;
    for (int n = 10000; n <= 1000000; n *= 10) {
        double duration_insert_edge_fstar = 0;
        double duration_insert_edge_spruce = 0;
        double duration_delete_edge_fstar = 0;
        double duration_delete_edge_spruce = 0;
        double duration_update_edge_fstar = 0;
        double duration_update_edge_spruce = 0;
        double duration_get_neighbours_fstar = 0;
        double duration_get_neighbours_spruce = 0;
        double duration_bfs_fstar = 0;
        double duration_bfs_spruce = 0;
        
        std::cout << "n = " << n << ", m = " << m << std::endl;
        std::vector<uint64_t> vertex_ids;
        std::unordered_set<uint64_t> vertex_set;
        max_uid = 0;
        for (int i = 0; i < n; i++) {
            uint64_t id = distribution(generator);
            while (vertex_set.find(id) != vertex_set.end()) {
                id = distribution(generator);
            }
            vertex_ids.push_back(id);
            vertex_set.insert(id);
            max_uid = std::max(max_uid, id);
        }
        for (int i = 0; i < num_trials; i++) {
            // std::cout << "Trial " << i + 1 << ":" << std::endl;
            RadixGraph G_fstar(d[now], a[now]);
            SpruceTransVer spruce;
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
                #pragma omp parallel for num_threads(num_threads)
                for (auto e : edges) {
                    G_fstar.InsertEdge(e.first.first, e.first.second, e.second);
                }
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> duration = end - start;
                duration_insert_edge_fstar += duration.count();
                
                start = std::chrono::high_resolution_clock::now();
                #pragma omp parallel for num_threads(num_threads)
                for (auto e : edges) {
                    SpruceTransVer::InsertEdge(spruce, {e.first.first, e.first.second, e.second});
                }
                end = std::chrono::high_resolution_clock::now();
                duration = end - start;
                duration_insert_edge_spruce += duration.count();
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
                #pragma omp parallel for num_threads(num_threads)
                for (auto e : edges_update) {
                    G_fstar.UpdateEdge(e.first.first, e.first.second, e.second);
                }
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> duration = end - start;
                duration_update_edge_fstar += duration.count();
                
                start = std::chrono::high_resolution_clock::now();
                #pragma omp parallel for num_threads(num_threads)
                for (auto e : edges_update) {
                    SpruceTransVer::UpdateEdge(spruce, {e.first.first, e.first.second, e.second});
                }
                end = std::chrono::high_resolution_clock::now();
                duration = end - start;
                duration_update_edge_spruce += duration.count();
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
                #pragma omp parallel for num_threads(num_threads)
                for (auto e : edges_delete) {
                    G_fstar.DeleteEdge(e.first.first, e.first.second);
                }
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> duration = end - start;
                duration_delete_edge_fstar += duration.count();
                
                start = std::chrono::high_resolution_clock::now();
                #pragma omp parallel for num_threads(num_threads)
                for (auto e : edges_delete) {
                    SpruceTransVer::DeleteEdge(spruce, e.first.first, e.first.second);
                }
                end = std::chrono::high_resolution_clock::now();
                duration = end - start;
                duration_delete_edge_spruce += duration.count();
            }

            // std::cout << "Delete done" << std::endl;

            // Get neighbours
            {
                auto start = std::chrono::high_resolution_clock::now();
                #pragma omp parallel for num_threads(num_threads)
                for (int j = 0; j < n; j++) {
                    std::vector<WeightedEdge> neighbours;
                    G_fstar.GetNeighbours(vertex_ids[j], neighbours);
                }
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> duration = end - start;
                duration_get_neighbours_fstar += duration.count();

                start = std::chrono::high_resolution_clock::now();
                #pragma omp parallel for num_threads(num_threads)
                for (int j = 0; j < n; j++) {
                    std::vector<SpruceTransVer::WeightedOutEdgeSimple> neighbours;
                    SpruceTransVer::get_neighbours(spruce, vertex_ids[j], neighbours);
                }
                end = std::chrono::high_resolution_clock::now();
                duration = end - start;
                duration_get_neighbours_spruce += duration.count();
            }

            // std::cout << "Get neighbors done" << std::endl;

            // BFS
            {
                auto start = std::chrono::high_resolution_clock::now();
                auto res_fstar = G_fstar.BFS(vertex_ids[0]);
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> duration = end - start;
                duration_bfs_fstar += duration.count();

                start = std::chrono::high_resolution_clock::now();
                auto res_spruce = spruce_bfs(spruce, vertex_ids[0]);
                end = std::chrono::high_resolution_clock::now();
                duration = end - start;
                duration_bfs_spruce += duration.count();

                std::sort(res_fstar.begin(), res_fstar.end());
                std::sort(res_spruce.begin(), res_spruce.end());
                if (res_fstar.size() != res_spruce.size()) {
                    std::cout << "BFS results wrong. Actual size = " << res_fstar.size() << ", expected size = " << res_spruce.size() << std::endl;
                    exit(-1);
                }
                for (int j = 0; j < res_fstar.size(); j++) {
                    if (res_fstar[j] != res_spruce[j]) {
                        std::cout << "BFS results wrong. Vertex " << res_fstar[j] << " found v.s. vertex " << res_spruce[j] << " expected." << std::endl;
                        exit(-1);
                    }
                }
            }

            // std::cout << "BFS done" << std::endl;
        }
        ++now;

        std::cout << "Average insertion time for RadixGraph: " << duration_insert_edge_fstar / num_trials << "s" << std::endl;
        std::cout << "Average insertion time for Spruce: " << duration_insert_edge_spruce / num_trials << "s" << std::endl;
        std::cout << std::endl;
        std::cout << "Average deletion time for RadixGraph: " << duration_delete_edge_fstar / num_trials << "s" << std::endl;
        std::cout << "Average deletion time for Spruce: " << duration_delete_edge_spruce / num_trials << "s" << std::endl;
        std::cout << std::endl;
        std::cout << "Average update time for RadixGraph: " << duration_update_edge_fstar / num_trials << "s" << std::endl;
        std::cout << "Average update time for Spruce: " << duration_update_edge_spruce / num_trials << "s" << std::endl;
        std::cout << std::endl;
        std::cout << "Average get neighbours time for RadixGraph: " << duration_get_neighbours_fstar / num_trials << "s" << std::endl;
        std::cout << "Average get neighbours time for Spruce: " << duration_get_neighbours_spruce / num_trials << "s" << std::endl;
        std::cout << std::endl;
        std::cout << "Average BFS time for RadixGraph: " << duration_bfs_fstar / num_trials << "s" << std::endl;
        std::cout << "Average BFS time for Spruce: " << duration_bfs_spruce / num_trials << "s" << std::endl;
    }
}