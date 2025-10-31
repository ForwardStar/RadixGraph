#include "../src/radixgraph.h"

int main() {
    RadixGraph G(4, 5); // 4 vertices, ID range from [0, 2^5-1]

    /* Insert edges. */
    G.InsertEdge(0, 1, 1); // Insert edge: from 0 to 1, weight 1
    G.InsertEdge(1, 5, 2);
    G.InsertEdge(5, 10, 3);
    G.InsertEdge(10, 31, 4);
    G.InsertEdge(31, 0, 5);

    /* Get neighbor example: get total weight of all edges. */
    int total_weight = 0;
    for (NodeID i = 0; i < 32; i++) {
        std::vector<WeightedEdge> neighbors;
        G.GetNeighbours(i, neighbors); // Get neighbors
        for (auto e : neighbors) total_weight += e.weight;
    }
    assert(total_weight == 15); // total weight is 15

    /* Delete edges. */
    G.DeleteEdge(0, 1); // Delete edge: from 0 to 1
    G.InsertEdge(0, 5, 1);

    /* Get neighbor vertex IDs. */
    std::vector<WeightedEdge> neighbors;
    G.GetNeighbours((NodeID)0, neighbors);
    // The only edge from 0 points to 5 with weight 1
    assert(neighbors.size() == 1);
    assert(neighbors[0].weight == 1);
    assert(G.GetExternalID(neighbors[0]) == 5); // Gets the actual ID
    auto logical_id = neighbors[0].idx; // Internal vertex ID
    assert(G.GetExternalID(logical_id) == 5); // Gets the actual ID
    return 0;
}
