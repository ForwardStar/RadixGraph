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

#define DEBUG_MODE 0
#define USE_SORT 1
#define USE_ART 0

#include "utils.h"
#if USE_SORT
    #include "sort.h"
#elif USE_ART
    #include "art.h"
#else
    #define MAX_VERTEX_ID 50000000 // Maximum vertex ID allowed in the system
    #include "vertex_array.h"
#endif

class RadixGraph {
    private:
        bool Insert(Vertex* src, int des, float weight, int delta_deg);
        WeightedEdgeArray* LogCompaction(WeightedEdgeArray* old_arr, WeightedEdgeArray* new_arr);
    public:
        static thread_local int thread_id_local;

        #if USE_SORT
            SORT* vertex_index = nullptr;
        #elif USE_ART
            ART* vertex_index = nullptr;
        #else
            VertexArray* vertex_index = nullptr;
        #endif

        SegmentedBitmap** bitmap = nullptr;
        bool is_mixed_workloads = false; // set to true when executing reads and writes concurrently
        bool is_sorted = false; // whether the neighbour list of each vertex is sorted
        int num_threads = 64; // 64 by default

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
            timestamp: if reading a snapshot, read the latest snapshot at the timestamp. */
        bool GetNeighbours(NodeID src, std::vector<WeightedEdge> &neighbours, bool is_snapshot=false, int timestamp=2147483647);
        bool GetNeighbours(Vertex* src, std::vector<WeightedEdge> &neighbours, bool is_snapshot=false, int timestamp=2147483647);
        /*  GetNeighboursByOffset(): get neighbours given a vertex dummy node;
            src: the offset of the source vertex, i.e., the logical ID of the vertex;
            neighbours: neighbour edges of src are stored in this array;
            is_snapshot: whether to read a graph snapshot or full edge logs;
            timestamp: if reading a snapshot, read the latest snapshot at the timestamp. */
        bool GetNeighboursByOffset(int src, std::vector<WeightedEdge> &neighbours, bool is_snapshot=false, int timestamp=2147483647);

        /* CreateSnapshots(): log compaction for all vertices;
            sort_neighbours: whether to sort neighbours in increasing internal IDs (useful for some graph analytics tasks requiring ordering);
            make_dense: whether to keep snapshot segment only. */
        void CreateSnapshots(bool sort_neighbours=false, bool make_dense=false);
        // Get the global timestamp and increment it.
        int GetGlobalTimestamp();
        // Set the number of threads in the system.
        void SetNumThreads(int nth=64);
        // Set the maximum vertex ID allowed in the system (only when using vertex array).
        #if !USE_SORT && !USE_ART
            void SetMaximumID(int max_id=MAX_VERTEX_ID);
        #endif
        // Get debug information of all vertices (only when DEBUG_MODE is true).
        struct DebugInfo {
            NodeID node = -1;
            int deg = 0;
            double t_total = 0, t_compact = 0;
        };
        std::vector<DebugInfo> GetDebugInfo() {
            #if DEBUG_MODE
                int n = vertex_index->cnt.load();
                std::vector<DebugInfo> res(n);
                for (int i = 0; i < n; i++) {
                    auto& node = vertex_index->vertex_table[i];
                    res[i] = {node.node, node.next.load()->deg, node.t_total.load(), node.t_compact.load()};
                }
                return res;
            #endif
            return {};
        }

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
            _num_threads: the number of threads allowed in the system;
            _max_vertex_id: the maximum vertex ID allowed in the system (when using vertex array).
            -----------------------------------------------------------------------------------------------------------------------
            If you are using ART or vertex array instead of SORT, you can leave the parameters empty to call RadixGraph() directly.
        */ 
        #if USE_SORT
            RadixGraph(int d=1, std::vector<int> _num_children={25}, int _num_threads=64);
        #elif USE_ART
            RadixGraph(int _num_threads=64);
        #else
            RadixGraph(int _num_threads=64, int _max_vertex_id=MAX_VERTEX_ID);
        #endif
        ~RadixGraph();
};

#endif