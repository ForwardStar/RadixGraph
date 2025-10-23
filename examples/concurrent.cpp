#include "../src/radixgraph.h"

int main() {
    const int num_threads = 5;
    global_info.is_mixed_workload = true; // remember to set this
    RadixGraph G(32, 5, num_threads); // 32 vertices, ID range [0,31]

    std::mt19937 rng(42);
    std::uniform_int_distribution<int> vertex_dist(0, 31);
    std::uniform_int_distribution<int> weight_dist(1, 10);

    auto start_time = std::chrono::steady_clock::now();
    auto end_time = start_time + std::chrono::seconds(10); // run for 10 seconds
  
    #pragma omp parallel num_threads(num_threads)
    {
        int tid = omp_get_thread_num();
        std::mt19937 thread_rng(rng()); // thread-local RNG
        while (std::chrono::steady_clock::now() < end_time) {
            if (tid % 2 == 0) { 
                // Insert a random edge
                NodeID u = vertex_dist(thread_rng);
                NodeID v = vertex_dist(thread_rng);
                int w = weight_dist(thread_rng);
                G.InsertEdge(u, v, w);
            }
            else {
                // Query 2-hop neighbors of a random vertex
                int timestamp = G.GetGlobalTimestamp(); // fix the timestamp for consistency
                NodeID u = vertex_dist(thread_rng);
                std::vector<WeightedEdge> neighbors;
                G.GetNeighbours(u, neighbors, timestamp);
                for (auto e : neighbors) {
                    std::vector<WeightedEdge> neighbors1;
                    G.GetNeighboursByOffset(e.idx, neighbors1, timestamp);
                }
            }
            // Optional small sleep to reduce CPU saturation
            if (tid == 0) std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
    return 0;
}
