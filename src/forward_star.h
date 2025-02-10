#ifndef FORWARDSTAR
#define FORWARDSTAR

#include "optimized_trie.h"
#define MAXINT 2147483647

class ForwardStar {
    private:
        bool Insert(DummyNode* src, DummyNode* des, double weight);      
    public:
        Trie* vertex_index = nullptr;
        int global_timestamp = 0;

        bool InsertEdge(NodeID src, NodeID des, double weight);
        bool UpdateEdge(NodeID src, NodeID des, double weight);
        bool DeleteEdge(NodeID src, NodeID des);
        bool GetNeighbours(NodeID src, std::vector<WeightedEdge> &neighbours, int timestamp=MAXINT);
        bool GetNeighbours(DummyNode* src, std::vector<WeightedEdge> &neighbours, int timestamp=MAXINT);

        std::vector<DummyNode*> BFS(NodeID src);
        std::vector<double> SSSP(NodeID src);

        ForwardStar(int d, std::vector<int> _num_children);
        ~ForwardStar();
};

#endif