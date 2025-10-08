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
#ifndef VARRAY_H
#define VARRAY_H

#include "utils.h"

class VertexArray {
    public:
        Vertex* vertex_table;
        AtomicBitmap* mtx;
        std::atomic<int> cnt;
        
        /*  InsertSimpleVertex(): insert a simple vertex into ART (only useful for benchmarking and testing);
            id: the vertex ID to be inserted. */
        void InsertSimpleVertex(NodeID id);
        /*  CheckExistence(): check whether a vertex exists in ART;
            id: the vertex ID to be retrieved; */
        bool CheckExistence(NodeID id);
        /*  RetrieveVertex(): retrieve a vertex from ART;
            id: the vertex ID to be retrieved;
            insert_mode: if set to true, insert the vertex if not found. */
        Vertex* RetrieveVertex(NodeID id, bool insert_mode=false);
        /*  DeleteVertex(): delete a vertex from ART;
            id: the vertex ID to be deleted. */
        bool DeleteVertex(NodeID id);

        VertexArray(int n);
        ~VertexArray();
};

#endif