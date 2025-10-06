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
#ifndef ART_H
#define ART_H

#include "utils.h"
#include <string_view>

#define UNODB_SPINLOCK_LOOP_VALUE 1

#include "../unodb/art_common.hpp"
#include "../unodb/olc_art.hpp"
#include "../unodb/qsbr.hpp"

using Db = unodb::olc_db<std::uint64_t, unodb::value_view>;

class ART {
    public:
        Db tree;
        tbb::concurrent_vector<Vertex> vertex_table;
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

        ART() {};
        ~ART() {};
};

#endif