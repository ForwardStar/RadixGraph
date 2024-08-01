#include "headers.h"
#include "adj_list.h"
#include "forward_star.h"

int main() {
    std::ios::sync_with_stdio(false);
    srand((int)time(NULL));
    double duration_insert_edge_adjlist = 0;
    double duration_insert_edge_fstar = 0;
    double duration_bfs_adjlist = 0;
    double duration_bfs_fstar = 0;

    for (int n = 10; n <= 100000; n *= 10) {
        std::cout << "n = " << n << ", m = " << 256000 << std::endl;
        for (int i = 0; i < 10; i++) {
            std::vector<std::pair<std::pair<uint64_t, uint64_t>, double>> edges;
            uint64_t u, v;
            double w;
            for (int i = 0; i < 256000; i++) {
                u = rand() % n;
                v = rand() % n;
                edges.push_back(std::make_pair(std::make_pair(u, v), 0.5));
            }

            AdjacencyList G_adj;
            auto start = std::chrono::high_resolution_clock::now();
            for (auto e : edges) {
                G_adj.InsertEdge(e.first.first, e.first.second, e.second);
            }
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> duration = end - start;
            duration_insert_edge_adjlist += duration.count();

            ForwardStar G_fstar;
            start = std::chrono::high_resolution_clock::now();
            for (auto e : edges) {
                G_fstar.InsertEdge(e.first.first, e.first.second, e.second);
            }
            end = std::chrono::high_resolution_clock::now();
            duration = end - start;
            duration_insert_edge_fstar += duration.count();

            start = std::chrono::high_resolution_clock::now();
            G_adj.BFS(1);
            end = std::chrono::high_resolution_clock::now();
            duration = end - start;
            duration_bfs_adjlist += duration.count();

            start = std::chrono::high_resolution_clock::now();
            G_fstar.BFS(1);
            end = std::chrono::high_resolution_clock::now();
            duration = end - start;
            duration_bfs_fstar += duration.count();
        }

        std::cout << "Average insertion time for adjacency list: " << duration_insert_edge_adjlist / 10 << "s" << std::endl;
        std::cout << "Average insertion time for forward star: " << duration_insert_edge_fstar / 10 << "s" << std::endl;
        std::cout << "Average BFS time for adjacency list: " << duration_bfs_adjlist / 10 << "s" << std::endl;
        std::cout << "Average BFS time for forward star: " << duration_bfs_fstar / 10 << "s" << std::endl;
    }
}