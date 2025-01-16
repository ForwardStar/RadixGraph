#ifndef FORWARDSTAR
#define FORWARDSTAR

#include "optimized_trie.h"

class ForwardStar {
    private:
        bool Insert(DummyNode* src, DummyNode* des, double weight, int type);      
    public:
        Trie* vertex_index = nullptr;
        int global_timestamp;

        bool InsertEdge(uint64_t src, uint64_t des, double weight);
        bool UpdateEdge(uint64_t src, uint64_t des, double weight);
        bool DeleteEdge(uint64_t src, uint64_t des);
        bool GetNeighbours(uint64_t src, std::vector<WeightedEdge> &neighbours);
        bool GetNeighbours(DummyNode* src, std::vector<WeightedEdge> &neighbours);

        std::vector<DummyNode*> BFS(uint64_t src);
        std::vector<double> SSSP(uint64_t src);

        ForwardStar(int d, std::vector<int> _num_children);
        ~ForwardStar();
};

#endif