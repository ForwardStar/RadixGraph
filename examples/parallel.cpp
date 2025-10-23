#include "../src/radixgraph.h"

int main() {
    RadixGraph G(4, 5, 5); // 4 vertices, ID range from [0, 2^5-1], 5 threads
    // Edges to be inserted
    std::vector<std::vector<int>> edges = {
        {0, 1, 1},
        {1, 5, 2},
        {5, 10, 3},
        {10, 31, 4},
        {31, 0, 5}
    };
    // Parallel insert
    #pragma omp parallel for num_threads(5)
    for (int i = 0; i < 5; i++) G.InsertEdge((NodeID)edges[i][0], (NodeID)edges[i][1], edges[i][2]);
    // Parallel get neighbors
    std::atomic<int> total_weight = 0;
    for (NodeID i = 0; i < 32; i++) {
        std::vector<WeightedEdge> neighbors;
        G.GetNeighbours(i, neighbors); // Get neighbors
        for (auto e : neighbors) total_weight += e.weight;
    }
    assert(total_weight == 15); // total weight is 15
    // Parallel delete
    #pragma omp parallel for num_threads(5)
    for (int i = 0; i < 5; i++) G.DeleteEdge((NodeID)edges[i][0], (NodeID)edges[i][1]);
    // Parallel get neighbors
    total_weight = 0;
    for (NodeID i = 0; i < 32; i++) {
        std::vector<WeightedEdge> neighbors;
        G.GetNeighbours(i, neighbors); // Get neighbors
        for (auto e : neighbors) total_weight += e.weight;
    }
    assert(total_weight == 0); // total weight is 0
    return 0;
}
