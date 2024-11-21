#include "headers.h"
#include "adj_list.h"
#include "forward_star.h"
#include "../Spruce/Spruce/src/spruce_transaction.h"

std::vector<uint64_t> spruce_bfs(SpruceTransVer& spruce, int n, uint64_t src) {
    std::unordered_set<uint64_t> visited_vertices;
    std::queue<uint64_t> Q;
    visited_vertices.insert(src);
    Q.push(src);
    std::vector<uint64_t> res;
    while (!Q.empty()) {
        auto u = Q.front();
        Q.pop();
        res.push_back(u);
        std::vector<SpruceTransVer::WeightedOutEdgeSimple> neighbours;
        SpruceTransVer::get_neighbours(spruce, u, neighbours);
        for (auto e : neighbours) {
            if (visited_vertices.find(e.des) == visited_vertices.end()) {
                visited_vertices.insert(e.des);
                Q.push(e.des);
            }
        }
    }
    return res;
}

int main() {
    std::ios::sync_with_stdio(false);
    srand((int)time(NULL));
    std::vector<int> d = {4, 4, 4, 4};
    std::vector<std::vector<int>> a = {
        {17, 8, 8, 7},
        {20, 7, 7, 6},
        {22, 6, 6, 6},
        {25, 5, 5, 5}
    };
    int m = 2560000;
    int num_trials = 5;
    
    std::default_random_engine generator;
    unsigned long long maximum = (1ull << 60) - 1;
    std::uniform_int_distribution distribution(0ull, maximum);
    int now = 0;
    for (int n = 1000; n <= 100000; n *= 10) {
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
        for (int i = 0; i < n; i++) {
            uint64_t id = distribution(generator);
            while (vertex_set.find(id) != vertex_set.end()) {
                id = distribution(generator);
            }
            vertex_ids.push_back(id);
            vertex_set.insert(id);
        }
        for (int i = 0; i < num_trials; i++) {
            ForwardStar G_fstar(d[now], a[now]);
            SpruceTransVer spruce;
            std::vector<std::pair<std::pair<uint64_t, uint64_t>, double>> edges;

            // Insert edges
            {
                uint64_t u, v;
                double w;
                for (int i = 0; i < n - 1; i++) {
                    edges.push_back(std::make_pair(std::make_pair(vertex_ids[i], vertex_ids[i + 1]), 0.5));
                }
                for (int i = 0; i < m - (n - 1); i++) {
                    int id1 = rand() % n, id2 = rand() % n;
                    u = vertex_ids[id1];
                    v = vertex_ids[id2];
                    edges.push_back(std::make_pair(std::make_pair(u, v), 0.5));
                }

                auto start = std::chrono::high_resolution_clock::now();
                #pragma omp parallel for num_threads(10)
                for (auto e : edges) {
                    G_fstar.InsertEdge(e.first.first, e.first.second, e.second);
                }
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> duration = end - start;
                duration_insert_edge_fstar += duration.count();
                
                start = std::chrono::high_resolution_clock::now();
                #pragma omp parallel for num_threads(10)
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
                for (int i = 0; i < 1000; i++) {
                    auto e = edges[rand() % m];
                    edges_update.push_back(e);
                }

                auto start = std::chrono::high_resolution_clock::now();
                #pragma omp parallel for num_threads(10)
                for (auto e : edges_update) {
                    G_fstar.UpdateEdge(e.first.first, e.first.second, e.second);
                }
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> duration = end - start;
                duration_update_edge_fstar += duration.count();
                
                start = std::chrono::high_resolution_clock::now();
                #pragma omp parallel for num_threads(10)
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
                for (int i = 0; i < 1000; i++) {
                    auto id = rand() % m;
                    while (idx_delete.find(id) != idx_delete.end() || id < n - 1) {
                        id = rand() % m;
                    }
                    idx_delete.insert(id);
                    auto e = edges[id];
                    edges_delete.push_back(e);
                }

                auto start = std::chrono::high_resolution_clock::now();
                #pragma omp parallel for num_threads(10)
                for (auto e : edges_delete) {
                    G_fstar.DeleteEdge(e.first.first, e.first.second);
                }
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> duration = end - start;
                duration_delete_edge_fstar += duration.count();
                
                start = std::chrono::high_resolution_clock::now();
                #pragma omp parallel for num_threads(10)
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
                #pragma omp parallel for num_threads(10)
                for (int j = 0; j < n; j++) {
                    std::vector<ForwardStar::WeightedEdge> neighbours;
                    G_fstar.GetNeighbours(vertex_ids[j], neighbours);
                }
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> duration = end - start;
                duration_get_neighbours_fstar += duration.count();

                start = std::chrono::high_resolution_clock::now();
                #pragma omp parallel for num_threads(10)
                for (int j = 0; j < n; j++) {
                    std::vector<SpruceTransVer::WeightedOutEdgeSimple> neighbours;
                    SpruceTransVer::get_neighbours(spruce, vertex_ids[j], neighbours);
                }
                end = std::chrono::high_resolution_clock::now();
                duration = end - start;
                duration_get_neighbours_spruce += duration.count();

                // Correctness check
                for (int j = 0; j < n; j++) {
                    std::vector<ForwardStar::WeightedEdge> neighbours_fstar;
                    std::vector<SpruceTransVer::WeightedOutEdgeSimple> neighbours_spruce;
                    G_fstar.GetNeighbours(vertex_ids[j], neighbours_fstar);
                    SpruceTransVer::get_neighbours(spruce, vertex_ids[j], neighbours_spruce);
                    std::sort(neighbours_fstar.begin(), neighbours_fstar.end(), [](ForwardStar::WeightedEdge a, ForwardStar::WeightedEdge b) {
                        return a.forward->node < b.forward->node;
                    });
                    std::sort(neighbours_spruce.begin(), neighbours_spruce.end(), [](SpruceTransVer::WeightedOutEdgeSimple a, SpruceTransVer::WeightedOutEdgeSimple b) {
                        return a.des < b.des;
                    });
                    assert(neighbours_fstar.size() == neighbours_spruce.size());
                    for (int k = 0; k < neighbours_fstar.size(); k++) {
                        assert(neighbours_fstar[k].forward->node == neighbours_spruce[k].des);
                    }
                }
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
                auto res_spruce = spruce_bfs(spruce, n, vertex_ids[0]);
                end = std::chrono::high_resolution_clock::now();
                duration = end - start;
                duration_bfs_spruce += duration.count();

                std::sort(res_fstar.begin(), res_fstar.end());
                std::sort(res_spruce.begin(), res_spruce.end());
                assert(res_fstar.size() == res_spruce.size());
                for (int j = 0; j < res_fstar.size(); j++) {
                    assert(res_fstar[j] == res_spruce[j]);
                }
            }

            // std::cout << "BFS done" << std::endl;
        }
        ++now;

        std::cout << "Average insertion time for forward star: " << duration_insert_edge_fstar / num_trials << "s" << std::endl;
        std::cout << "Average insertion time for spruce: " << duration_insert_edge_spruce / num_trials << "s" << std::endl;
        std::cout << std::endl;
        std::cout << "Average deletion time for forward star: " << duration_delete_edge_fstar / num_trials << "s" << std::endl;
        std::cout << "Average deletion time for spruce: " << duration_delete_edge_spruce / num_trials << "s" << std::endl;
        std::cout << std::endl;
        std::cout << "Average update time for forward star: " << duration_update_edge_fstar / num_trials << "s" << std::endl;
        std::cout << "Average update time for spruce: " << duration_update_edge_spruce / num_trials << "s" << std::endl;
        std::cout << std::endl;
        std::cout << "Average get neighbours time for forward star: " << duration_get_neighbours_fstar / num_trials << "s" << std::endl;
        std::cout << "Average get neighbours time for spruce: " << duration_get_neighbours_spruce / num_trials << "s" << std::endl;
        std::cout << std::endl;
        std::cout << "Average BFS time for forward star: " << duration_bfs_fstar / num_trials << "s" << std::endl;
        std::cout << "Average BFS time for spruce: " << duration_bfs_spruce / num_trials << "s" << std::endl;
    }
}