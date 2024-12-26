#ifndef FORWARDSTAR
#define FORWARDSTAR

#include "headers.h"
#include "optimized_trie.h"

class ForwardStar {
    private:
        bool Insert(DummyNode* src, DummyNode* des, double weight, int flag);      
    public:
        Trie* vertex_index = nullptr;
        std::atomic<int> cnt;
        int global_timestamp;
        std::vector<int> thread_pool;

        bool InsertEdge(uint64_t src, uint64_t des, double weight);

        bool UpdateEdge(uint64_t src, uint64_t des, double weight);

        bool DeleteEdge(uint64_t src, uint64_t des);

        bool GetNeighbours(uint64_t src, std::vector<WeightedEdge> &neighbours);

        bool GetNeighbours(DummyNode* src, std::vector<WeightedEdge> &neighbours);

        std::vector<DummyNode*> BFS(uint64_t src);

        ForwardStar(int d, std::vector<int> _num_children);
        ~ForwardStar();
};

#endif