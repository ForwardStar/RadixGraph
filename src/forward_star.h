#ifndef FORWARDSTAR
#define FORWARDSTAR

#include "optimized_trie.h"

class ForwardStar {
    private:
        bool Insert(DummyNode* src, DummyNode* des, double weight);      
    public:
        Trie* vertex_index = nullptr;

        bool InsertEdge(NodeID src, NodeID des, double weight);
        bool UpdateEdge(NodeID src, NodeID des, double weight);
        bool DeleteEdge(NodeID src, NodeID des);
        bool GetNeighbours(NodeID src, std::vector<WeightedEdge> &neighbours, int timestamp=-1);
        bool GetNeighbours(DummyNode* src, std::vector<WeightedEdge> &neighbours, int timestamp=-1);

        std::vector<DummyNode*> BFS(NodeID src);
        std::vector<double> SSSP(NodeID src);

        ForwardStar(int d, std::vector<int> _num_children);
        ~ForwardStar();
};

#endif