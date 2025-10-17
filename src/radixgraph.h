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

#define USE_SORT 0
#define USE_ART 1
#define USE_EDGE_CHAIN 1

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
            timestamp: if reading a snapshot, read the latest snapshot at the timestamp. */
        bool GetNeighbours(NodeID src, std::vector<WeightedEdge> &neighbours, int timestamp=2147483647);
        bool GetNeighbours(Vertex* src, std::vector<WeightedEdge> &neighbours, int timestamp=2147483647);
        /*  GetNeighboursByOffset(): get neighbours given a vertex dummy node;
            src: the offset of the source vertex, i.e., the logical ID of the vertex;
            neighbours: neighbour edges of src are stored in this array;
            timestamp: if reading a snapshot, read the latest snapshot at the timestamp. */
        bool GetNeighboursByOffset(int src, std::vector<WeightedEdge> &neighbours, int timestamp=2147483647);
        /*  GetNumVertices(): get the number of vertices in the graph;
            Returns the number of vertices. */
        int GetNumVertices() {
            return vertex_index->cnt.load();
        }

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

        /*  RadixGraph(): initialization of a RadixGraph instance;
            n: exact or estimated number of vertices to be stored (when using SORT);
            bit_length: bit length of vertex IDs (when using SORT);
            _num_threads: the number of threads allowed in the system;
            _max_vertex_id: the maximum vertex ID allowed in the system (when using vertex array);
            L: the segment size of the segmented bitmap. */ 
        #if USE_SORT
            RadixGraph(int n, int bit_length, int _num_threads=64, int L=1000000);
            // Alternatively, initialize with your custom fanout configuration
            RadixGraph(std::vector<int> _num_bits, int _num_threads=64, int L=1000000);
        #elif USE_ART
            RadixGraph(int _num_threads=64, int L=1000000);
        #else
            RadixGraph(int _num_threads=64, int _max_vertex_id=MAX_VERTEX_ID, int L=1000000);
        #endif
        ~RadixGraph();
};

#endif