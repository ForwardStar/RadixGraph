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
#ifndef SORT_H
#define SORT_H

#include "utils.h"

namespace Optimizer {
    /*  OptimalFanout(): the optimal fanout array a for SORT;
        n: number of vertices;
        bit_length: bit length of vertex IDs;
        l: number of layers in SORT;
        return: optimal fanout array a. */
    std::vector<int> OptimalFanout(int n, int bit_length, int l);

    /*  Prune(): prune the zero ai values in the fanout array;
        a: the fanout array to be pruned;
        return: pruned fanout array. */
    std::vector<int> Prune(std::vector<int> a);
};

class SORT {
    private:
        struct SORTNode {
            uint64_t* children = nullptr;
            AtomicBitmap* mtx = nullptr;

            ~SORTNode() {
              if (mtx) delete mtx;
              if (children) delete [] children;
            }
        };
        inline Vertex* InsertVertex(SORTNode* current, NodeID id, int d);
    public:
        SORTNode* root = nullptr;
        tbb::concurrent_vector<Vertex> vertex_table;
        std::vector<int> num_bits, sum_bits;
        int depth = 0, space = 0;
        std::atomic<int> cnt;
        std::atomic_flag mtx;
        #if ENABLE_GARBAGE_COLLECTION
            tbb::concurrent_queue<int> deleted_slots;
        #endif

        /*  InsertSimpleVertex(): insert a simple vertex into SORT (only useful for benchmarking and testing);
            id: the vertex ID to be inserted. */
        void InsertSimpleVertex(NodeID id);
        /*  CheckExistence(): check whether a vertex exists in SORT;
            id: the vertex ID to be retrieved. */
        bool CheckExistence(NodeID id);
        /*  RetrieveVertex(): retrieve a vertex from SORT;
            id: the vertex ID to be retrieved;
            insert_mode: if set to true, insert the vertex if not found. */
        Vertex* RetrieveVertex(NodeID id, bool insert_mode=false);
        /*  DeleteVertex(): delete a vertex from SORT;
            id: the vertex ID to be deleted. */
        bool DeleteVertex(NodeID id);
        /*  Transform(): transform the SORT to another configuration;
            d: number of layers;
            _num_bits: values of ai;
            vertex_set: set of current inserted vertices. */
        void Transform(int d, std::vector<int> _num_bits, std::vector<uint64_t>& vertex_set);

        /*  Size(): number of pointers created in SORT. */
        long long Size();

        SORT() {}
        /*  SORT(): initialization of a SORT instance;
            n: number of vertices;
            bit_length: bit length of vertex IDs;
            l: number of layers in SORT;
            prune: whether to prune the 0 values in the optimal fanout array. */
        SORT(int n, int bit_length, int l, bool prune=true);
        /*  SORT(): initialization of a SORT instance;
            d: number of layers;
            _num_bits: values of ai. */
        SORT(int d, int _num_bits[]);
        /*  SORT(): initialization of a SORT instance;
            _num_bits: values of ai. */
        SORT(std::vector<int> _num_bits);
        ~SORT();
};

#endif