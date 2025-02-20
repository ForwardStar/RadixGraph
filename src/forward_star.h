/*
 * Copyright (C) 2025 Haoxuan Xie
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef FORWARDSTAR
#define FORWARDSTAR

#include "optimized_trie.h"

class ForwardStar {
    private:
        bool Insert(DummyNode* src, DummyNode* des, double weight);      
    public:
        Trie* vertex_index = nullptr;
        bool enable_query = true;
        std::atomic<int>* degree = nullptr;
        std::atomic<int> cnt;

        bool InsertEdge(NodeID src, NodeID des, double weight);
        bool UpdateEdge(NodeID src, NodeID des, double weight);
        bool DeleteEdge(NodeID src, NodeID des);
        bool GetNeighbours(NodeID src, std::vector<WeightedEdge> &neighbours, int timestamp=-1);
        bool GetNeighbours(DummyNode* src, std::vector<WeightedEdge> &neighbours, int timestamp=-1);

        std::vector<DummyNode*> BFS(NodeID src);
        std::vector<double> SSSP(NodeID src);

        ForwardStar(int d, std::vector<int> _num_children, bool _enable_query=true);
        ~ForwardStar();
};

#endif