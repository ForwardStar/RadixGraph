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
#ifndef TRIE
#define TRIE

#include "utils.h"

class SORT {
    public:
        typedef struct _sort_node {
            uint64_t* children = nullptr;
            AtomicBitmap* mtx = nullptr;

            ~_sort_node() {
              if (mtx) delete mtx;
              if (children) delete [] children;
            }
        } SORTNode;

        SORTNode* root = nullptr;
        tbb::concurrent_vector<Vertex> vertex_table;
        std::vector<int> num_bits, sum_bits;
        int depth = 0, space = 0;
        std::atomic<int> cnt;
        std::atomic_flag mtx;

        /*  InsertSimpleVertex(): insert a simple vertex into SORT (only useful for benchmarking and testing);
            id: the vertex ID to be inserted. */
        void InsertSimpleVertex(NodeID id);
        /*  CheckExistence(): check whether a vertex exists in SORT;
            id: the vertex ID to be retrieved; */
        bool CheckExistence(NodeID id);
        /*  InsertVertex(): insert a vertex into SORT;
            current: using a recursive algorithm, current represents currently traversed tree node (initially be root);
            id: the vertex ID to be inserted;
            d: the layer of currently traversed tree node. */
        inline Vertex* InsertVertex(SORTNode* current, NodeID id, int d);
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

        long long size();

        SORT() {}
        SORT(int d, int _num_bits[]);
        SORT(int d, std::vector<int> _num_bits);
        ~SORT();
};

#endif