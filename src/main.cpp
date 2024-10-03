#include "headers.h"
#include "adj_list.h"
#include "forward_star.h"
#include "../Spruce/Spruce/src/spruce_transaction.h"

void spruce_bfs(SpruceTransVer& spruce, int n, uint64_t src) {
    std::unordered_set<uint64_t> visited_vertices;
    std::queue<uint64_t> Q;
    visited_vertices.insert(src);
    Q.push(src);
    while (!Q.empty()) {
        auto u = Q.front();
        Q.pop();
        std::vector<SpruceTransVer::WeightedOutEdgeSimple> neighbours;
        SpruceTransVer::get_neighbours(spruce, u, neighbours);
        for (auto e : neighbours) {
            if (visited_vertices.find(e.des) == visited_vertices.end()) {
                visited_vertices.insert(e.des);
                Q.push(e.des);
            }
        }
    }
}

int main() {
    std::ios::sync_with_stdio(false);
    srand((int)time(NULL));
    double duration_insert_edge_adjlinkedlist = 0;
    double duration_insert_edge_adjarraylist = 0;
    double duration_insert_edge_chainedfstar = 0;
    double duration_insert_edge_arrayfstar = 0;
    double duration_insert_edge_spruce = 0;
    double duration_delete_edge_adjlinkedlist = 0;
    double duration_delete_edge_adjarraylist = 0;
    double duration_delete_edge_chainedfstar = 0;
    double duration_delete_edge_arrayfstar = 0;
    double duration_delete_edge_spruce = 0;
    double duration_update_edge_adjlinkedlist = 0;
    double duration_update_edge_adjarraylist = 0;
    double duration_update_edge_chainedfstar = 0;
    double duration_update_edge_arrayfstar = 0;
    double duration_update_edge_spruce = 0;
    double duration_get_neighbours_adjlinkedlist = 0;
    double duration_get_neighbours_adjarraylist = 0;
    double duration_get_neighbours_chainedfstar = 0;
    double duration_get_neighbours_arrayfstar = 0;
    double duration_get_neighbours_spruce = 0;
    double duration_bfs_adjlinkedlist = 0;
    double duration_bfs_adjarraylist = 0;
    double duration_bfs_chainedfstar = 0;
    double duration_bfs_arrayfstar = 0;
    double duration_bfs_spruce = 0;

    for (int n = 10; n <= 100000; n *= 10) {
        std::cout << "n = " << n << ", m = " << 256000 << std::endl;
        for (int i = 0; i < 10; i++) {
            AdjacencyLinkedList G_adj_linked;
            ChainedForwardStar G_chained_fstar;
            AdjacencyArrayList G_adj_array;
            ArrayForwardStar G_array_fstar;
            SpruceTransVer spruce;

            // Insert edges
            {
                std::vector<std::pair<std::pair<uint64_t, uint64_t>, double>> edges;
                uint64_t u, v;
                double w;
                for (int i = 0; i < 256000; i++) {
                    u = rand() % n;
                    v = rand() % n;
                    edges.push_back(std::make_pair(std::make_pair(u, v), 0.5));
                }

                auto start = std::chrono::high_resolution_clock::now();
                for (auto e : edges) {
                    G_adj_linked.InsertEdge(e.first.first, e.first.second, e.second);
                }
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> duration = end - start;
                duration_insert_edge_adjlinkedlist += duration.count();

                start = std::chrono::high_resolution_clock::now();
                for (auto e : edges) {
                    G_chained_fstar.InsertEdge(e.first.first, e.first.second, e.second);
                }
                end = std::chrono::high_resolution_clock::now();
                duration = end - start;
                duration_insert_edge_chainedfstar += duration.count();

                start = std::chrono::high_resolution_clock::now();
                for (auto e : edges) {
                    G_adj_array.InsertEdge(e.first.first, e.first.second, e.second);
                }
                end = std::chrono::high_resolution_clock::now();
                duration = end - start;
                duration_insert_edge_adjarraylist += duration.count();

                start = std::chrono::high_resolution_clock::now();
                for (auto e : edges) {
                    G_array_fstar.InsertEdge(e.first.first, e.first.second, e.second);
                }
                end = std::chrono::high_resolution_clock::now();
                duration = end - start;
                duration_insert_edge_arrayfstar += duration.count();
                
                start = std::chrono::high_resolution_clock::now();
                for (auto e : edges) {
                    SpruceTransVer::InsertEdge(spruce, {e.first.first, e.first.second, e.second});
                }
                end = std::chrono::high_resolution_clock::now();
                duration = end - start;
                duration_insert_edge_spruce += duration.count();
            }

            // Delete edges
            {         
                std::vector<std::pair<std::pair<uint64_t, uint64_t>, double>> edges;
                uint64_t u, v;
                for (int i = 0; i < 1000; i++) {
                    u = rand() % n;
                    v = rand() % n;
                    edges.push_back(std::make_pair(std::make_pair(u, v), 0.5));
                }

                auto start = std::chrono::high_resolution_clock::now();
                for (auto e : edges) {
                    G_adj_linked.DeleteEdge(e.first.first, e.first.second);
                }
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> duration = end - start;
                duration_delete_edge_adjlinkedlist += duration.count();

                start = std::chrono::high_resolution_clock::now();
                for (auto e : edges) {
                    G_chained_fstar.DeleteEdge(e.first.first, e.first.second);
                }
                end = std::chrono::high_resolution_clock::now();
                duration = end - start;
                duration_delete_edge_chainedfstar += duration.count();

                start = std::chrono::high_resolution_clock::now();
                for (auto e : edges) {
                    G_adj_array.DeleteEdge(e.first.first, e.first.second);
                }
                end = std::chrono::high_resolution_clock::now();
                duration = end - start;
                duration_delete_edge_adjarraylist += duration.count();

                start = std::chrono::high_resolution_clock::now();
                // #pragma omp parallel for num_threads(10)
                for (auto e : edges) {
                    G_array_fstar.DeleteEdge(e.first.first, e.first.second);
                }
                end = std::chrono::high_resolution_clock::now();
                duration = end - start;
                duration_delete_edge_arrayfstar += duration.count();
                
                start = std::chrono::high_resolution_clock::now();
                for (auto e : edges) {
                    SpruceTransVer::DeleteEdge(spruce, e.first.first, e.first.second);
                }
                end = std::chrono::high_resolution_clock::now();
                duration = end - start;
                duration_delete_edge_spruce += duration.count();
            }

            // Update edges
            {         
                std::vector<std::pair<std::pair<uint64_t, uint64_t>, double>> edges;
                uint64_t u, v;
                for (int i = 0; i < 1000; i++) {
                    u = rand() % n;
                    v = rand() % n;
                    edges.push_back(std::make_pair(std::make_pair(u, v), 0.5));
                }

                auto start = std::chrono::high_resolution_clock::now();
                for (auto e : edges) {
                    G_adj_linked.UpdateEdge(e.first.first, e.first.second, e.second);
                }
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> duration = end - start;
                duration_update_edge_adjlinkedlist += duration.count();

                start = std::chrono::high_resolution_clock::now();
                for (auto e : edges) {
                    G_chained_fstar.UpdateEdge(e.first.first, e.first.second, e.second);
                }
                end = std::chrono::high_resolution_clock::now();
                duration = end - start;
                duration_update_edge_chainedfstar += duration.count();

                start = std::chrono::high_resolution_clock::now();
                for (auto e : edges) {
                    G_adj_array.UpdateEdge(e.first.first, e.first.second, e.second);
                }
                end = std::chrono::high_resolution_clock::now();
                duration = end - start;
                duration_update_edge_adjarraylist += duration.count();

                start = std::chrono::high_resolution_clock::now();
                for (auto e : edges) {
                    G_array_fstar.UpdateEdge(e.first.first, e.first.second, e.second);
                }
                end = std::chrono::high_resolution_clock::now();
                duration = end - start;
                duration_update_edge_arrayfstar += duration.count();
                
                start = std::chrono::high_resolution_clock::now();
                for (auto e : edges) {
                    SpruceTransVer::UpdateEdge(spruce, {e.first.first, e.first.second, e.second});
                }
                end = std::chrono::high_resolution_clock::now();
                duration = end - start;
                duration_update_edge_spruce += duration.count();
            }

            // Get neighbours
            {
                auto start = std::chrono::high_resolution_clock::now();
                for (int u = 0; u < n; u++) {
                    std::vector<AdjacencyLinkedList::WeightedEdge*> neighbours;
                    G_adj_linked.GetNeighbours(u, neighbours);
                }
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> duration = end - start;
                duration_get_neighbours_adjlinkedlist += duration.count();

                start = std::chrono::high_resolution_clock::now();
                for (int u = 0; u < n; u++) {
                    std::vector<ChainedForwardStar::WeightedEdge*> neighbours;
                    G_chained_fstar.GetNeighbours(u, neighbours);
                }
                end = std::chrono::high_resolution_clock::now();
                duration = end - start;
                duration_get_neighbours_chainedfstar += duration.count();

                start = std::chrono::high_resolution_clock::now();
                for (int u = 0; u < n; u++) {
                    std::vector<AdjacencyArrayList::WeightedEdge*> neighbours;
                    G_adj_array.GetNeighbours(u, neighbours);
                }
                end = std::chrono::high_resolution_clock::now();
                duration = end - start;
                duration_get_neighbours_adjarraylist += duration.count();

                start = std::chrono::high_resolution_clock::now();
                for (int u = 0; u < n; u++) {
                    std::vector<ArrayForwardStar::WeightedEdge*> neighbours;
                    G_array_fstar.GetNeighbours(u, neighbours);
                }
                end = std::chrono::high_resolution_clock::now();
                duration = end - start;
                duration_get_neighbours_arrayfstar += duration.count();

                start = std::chrono::high_resolution_clock::now();
                for (int u = 0; u < n; u++) {
                    std::vector<SpruceTransVer::WeightedOutEdgeSimple> neighbours;
                    SpruceTransVer::get_neighbours(spruce, u, neighbours);
                }
                end = std::chrono::high_resolution_clock::now();
                duration = end - start;
                duration_get_neighbours_spruce += duration.count();
            }

            // BFS
            {
                auto start = std::chrono::high_resolution_clock::now();
                G_adj_linked.BFS(1);
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> duration = end - start;
                duration_bfs_adjlinkedlist += duration.count();

                start = std::chrono::high_resolution_clock::now();
                G_chained_fstar.BFS(1);
                end = std::chrono::high_resolution_clock::now();
                duration = end - start;
                duration_bfs_chainedfstar += duration.count();

                start = std::chrono::high_resolution_clock::now();
                G_adj_array.BFS(1);
                end = std::chrono::high_resolution_clock::now();
                duration = end - start;
                duration_bfs_adjarraylist += duration.count();

                start = std::chrono::high_resolution_clock::now();
                G_array_fstar.BFS(1);
                end = std::chrono::high_resolution_clock::now();
                duration = end - start;
                duration_bfs_arrayfstar += duration.count();

                start = std::chrono::high_resolution_clock::now();
                spruce_bfs(spruce, n, 1);
                end = std::chrono::high_resolution_clock::now();
                duration = end - start;
                duration_bfs_spruce += duration.count();
            }
        }

        std::cout << "Average insertion time for adjacency linked list: " << duration_insert_edge_adjlinkedlist / 10 << "s" << std::endl;
        std::cout << "Average insertion time for chained forward star: " << duration_insert_edge_chainedfstar / 10 << "s" << std::endl;
        std::cout << "Average insertion time for adjacency array list: " << duration_insert_edge_adjarraylist / 10 << "s" << std::endl;
        std::cout << "Average insertion time for array forward star: " << duration_insert_edge_arrayfstar / 10 << "s" << std::endl;
        std::cout << "Average insertion time for spruce: " << duration_insert_edge_spruce / 10 << "s" << std::endl;
        std::cout << std::endl;
        std::cout << "Average deletion time for adjacency linked list: " << duration_delete_edge_adjlinkedlist / 10 << "s" << std::endl;
        std::cout << "Average deletion time for chained forward star: " << duration_delete_edge_chainedfstar / 10 << "s" << std::endl;
        std::cout << "Average deletion time for adjacency array list: " << duration_delete_edge_adjarraylist / 10 << "s" << std::endl;
        std::cout << "Average deletion time for array forward star: " << duration_delete_edge_arrayfstar / 10 << "s" << std::endl;
        std::cout << "Average deletion time for spruce: " << duration_delete_edge_spruce / 10 << "s" << std::endl;
        std::cout << std::endl;
        std::cout << "Average update time for adjacency linked list: " << duration_update_edge_adjlinkedlist / 10 << "s" << std::endl;
        std::cout << "Average update time for chained forward star: " << duration_update_edge_chainedfstar / 10 << "s" << std::endl;
        std::cout << "Average update time for adjacency array list: " << duration_update_edge_adjarraylist / 10 << "s" << std::endl;
        std::cout << "Average update time for array forward star: " << duration_update_edge_arrayfstar / 10 << "s" << std::endl;
        std::cout << "Average update time for spruce: " << duration_update_edge_spruce / 10 << "s" << std::endl;
        std::cout << std::endl;
        std::cout << "Average get neighbours time for adjacency linked list: " << duration_get_neighbours_adjlinkedlist / 10 << "s" << std::endl;
        std::cout << "Average get neighbours time for chained forward star: " << duration_get_neighbours_chainedfstar / 10 << "s" << std::endl;
        std::cout << "Average get neighbours time for adjacency array list: " << duration_get_neighbours_adjarraylist / 10 << "s" << std::endl;
        std::cout << "Average get neighbours time for array forward star: " << duration_get_neighbours_arrayfstar / 10 << "s" << std::endl;
        std::cout << "Average get neighbours time for spruce: " << duration_get_neighbours_spruce / 10 << "s" << std::endl;
        std::cout << std::endl;
        std::cout << "Average BFS time for adjacency linked list: " << duration_bfs_adjlinkedlist / 10 << "s" << std::endl;
        std::cout << "Average BFS time for chained forward star: " << duration_bfs_chainedfstar / 10 << "s" << std::endl;
        std::cout << "Average BFS time for adjacency array list: " << duration_bfs_adjarraylist / 10 << "s" << std::endl;
        std::cout << "Average BFS time for array forward star: " << duration_bfs_arrayfstar / 10 << "s" << std::endl;
        std::cout << "Average BFS time for spruce: " << duration_bfs_spruce / 10 << "s" << std::endl;
    }
}