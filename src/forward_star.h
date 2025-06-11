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
        AtomicBitmap** bitmap = nullptr;

        /*  InsertEdge(): insert an edge to RadixGraph;
            src: the source vertex of the edge;
            des: the destination vertex of the edge;
            weight: the weight of the edge. */
        bool InsertEdge(NodeID src, NodeID des, double weight);
        /*  UpdateEdge(): update an edge to RadixGraph;
            src: the source vertex of the edge;
            des: the destination vertex of the edge;
            weight: the updated weight of the edge. */
        bool UpdateEdge(NodeID src, NodeID des, double weight);
        /*  DeleteEdge(): delete an edge from RadixGraph;
            src: the source vertex of the edge;
            des: the destination vertex of the edge. */
        bool DeleteEdge(NodeID src, NodeID des);
        /*  GetNeighbours(): get neighbours given a vertex ID;
            src: the target vertex ID;
            neighbours: neighbour edges of src are stored in this array;
            timestamp: the version (size) of the edge array, -1 means retrieving the latest version. */
        bool GetNeighbours(NodeID src, std::vector<WeightedEdge> &neighbours, int timestamp=-1);
        /*  GetNeighboursByOffset(): get neighbours given a vertex dummy node;
            src: the offset of the source vertex;
            neighbours: neighbour edges of src are stored in this array;
            timestamp: the version (size) of the edge array, -1 means retrieving the latest version. */
        bool GetNeighboursByOffset(int src, std::vector<WeightedEdge> &neighbours, int timestamp=-1);

        /*  BFS(): get all reachable vertices from a given vertex ID (single-threaded);
            src: the source vertex ID;
            Returns an array of dummy nodes containing all reachable vertices.
        */
        std::vector<uint64_t> BFS(NodeID src);
        /*  SSSP(): get shortest distances from a given vertex ID (single-threaded);
            src: the source vertex ID;
            Returns an array of numbers containing shortest distances to all vertices.
        */
        std::vector<double> SSSP(NodeID src);

        /*  ForwardStar(): initialization of a RadixGraph instance;
            d: depth of the SORT (vertex index);
            _num_children: a_i for each layer i, meaning a node in the i-th layer has 2^(a_i) child pointers;
            enable_query: whether to enable querying components. */ 
        ForwardStar(int d, std::vector<int> _num_children, bool _enable_query=true);
        ~ForwardStar();
};

#endif