#include "headers.h"
#include "adj_list.h"
#include "forward_star.h"

int main() {
    std::ios::sync_with_stdio(false);
    freopen("graph.txt", "r", stdin);
    std::vector<std::pair<std::pair<uint64_t, uint64_t>, double>> edges;
    uint64_t u, v;
    double w;
    while (std::cin >> u >> v >> w) {
        edges.push_back(std::make_pair(std::make_pair(u, v), w));
    }

    AdjacencyList G_adj;
    ForwardStar G_fstar;
}