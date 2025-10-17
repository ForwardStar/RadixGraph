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
#include "vertex_array.h"

void VertexArray::InsertSimpleVertex(NodeID id) {
    RetrieveVertex(id, true);
}

bool VertexArray::CheckExistence(NodeID id) {
    return vertex_table[id].idx != -1;
}

Vertex* VertexArray::RetrieveVertex(NodeID id, bool insert_mode) {
    if (vertex_table[id].idx == -1) {
        if (insert_mode) {
            mtx->set_bit_atomic(id);
            if (vertex_table[id].idx == -1) {
                vertex_table[id].next = new WeightedEdgeArray(8);
                vertex_table[id].node = id;
                vertex_table[id].idx = id;
            }
            mtx->clear_bit(id);
        }
        else {
            return nullptr;
        }
    }
    return &vertex_table[id];
}

bool VertexArray::DeleteVertex(NodeID id) {
    Vertex* tmp = RetrieveVertex(id);
    if (tmp == nullptr) {
        return false;
    }
    // Do not delete the vertex directly; defer it to get_neighbor and log compaction.
    tmp->del_time = global_info.global_timestamp;
    return true;
}

VertexArray::VertexArray(int n) {
    vertex_table = new Vertex[n];
    mtx = new AtomicBitmap(n);
}

VertexArray::~VertexArray() {
    delete [] vertex_table;
    delete mtx;
}