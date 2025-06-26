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
#ifndef RG
#define RG

#include "optimized_trie.h"

class RadixGraph {
    private:
        bool Insert(DummyNode* src, int des, float weight, int delta_deg);
        WeightedEdgeArray* LogCompaction(WeightedEdgeArray* old_arr, WeightedEdgeArray* new_arr);
    public:
        static std::atomic<int> global_timestamp;
        static thread_local int thread_id_local;

        SORT* vertex_index = nullptr;
        AtomicBitmap** bitmap = nullptr;
 
        /* Sample edge and vertex;
           See detail structures in ``optimized_trie.h``.
        */
        WeightedEdge sample_edge = {/* weight */0.5, /* offset */2};
        DummyNode sample_vertex = {/* ID */10, /* Offset */0, /* Del_time */0, /* mtx */0, /* EdgeArr */nullptr};

        /*  InsertEdge(): insert an edge to RadixGraph;
            src: the source vertex of the edge;
            des: the destination vertex of the edge;
            weight: the weight of the edge. */
        bool InsertEdge(NodeID src, NodeID des, float weight);
        /*  UpdateEdge(): update an edge to RadixGraph;
            src: the source vertex of the edge;
            des: the destination vertex of the edge;
            weight: the updated weight of the edge. */
        bool UpdateEdge(NodeID src, NodeID des, float weight);
        /*  DeleteEdge(): delete an edge from RadixGraph;
            src: the source vertex of the edge;
            des: the destination vertex of the edge. */
        bool DeleteEdge(NodeID src, NodeID des);
        /*  GetNeighbours(): get neighbours given a vertex ID;
            src: the target vertex ID or dummy node pointer;
            neighbours: neighbour edges of src are stored in this array;
            is_snapshot: whether to read a graph snapshot or full edge logs;
            timestamp: if reading a snapshot, read the latest snapshot at the timestamp.
        */
        bool GetNeighbours(NodeID src, std::vector<WeightedEdge> &neighbours, bool is_snapshot=true, int timestamp=2147483647);
        bool GetNeighbours(DummyNode* src, std::vector<WeightedEdge> &neighbours, bool is_snapshot=true, int timestamp=2147483647);
        /*  GetNeighboursByOffset(): get neighbours given a vertex dummy node;
            src: the offset of the source vertex, i.e., the logical ID of the vertex;
            neighbours: neighbour edges of src are stored in this array;
            is_snapshot: whether to read a graph snapshot or full edge logs;
            timestamp: if reading a snapshot, read the latest snapshot at the timestamp.
        */
        bool GetNeighboursByOffset(int src, std::vector<WeightedEdge> &neighbours, bool is_snapshot=true, int timestamp=2147483647);

        /*  BFS(): get all reachable vertices from a given vertex ID (single-threaded);
            src: the source vertex ID;
            Returns an array of all reachable vertex IDs.
        */
        std::vector<uint64_t> BFS(NodeID src);
        /*  SSSP(): get shortest distances from a given vertex ID (single-threaded);
            src: the source vertex ID;
            Returns an array of numbers containing shortest distances to all vertices.
        */
        std::vector<double> SSSP(NodeID src);

        /*  RadixGraph(): initialization of a RadixGraph instance;
            d: depth of the SORT (vertex index);
            _num_children: a_i for each layer i, meaning a node in the i-th layer has 2^(a_i) child pointers;
            enable_query: whether to enable querying components (bitmaps). */ 
        RadixGraph(int d, std::vector<int> _num_children);
        ~RadixGraph();
};

#endif