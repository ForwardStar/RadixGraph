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
#include "art.h"

constexpr std::string_view test_value = "test";

[[nodiscard, gnu::pure]] unodb::value_view from_string_view(
    std::string_view sv) {
  return {reinterpret_cast<const std::byte*>(sv.data()), sv.length()};
}

// Funtion to convert std::optional<value_view> to Vertex*
Vertex* optional_value_view_to_vertex_ptr(const std::optional<unodb::value_view>& opt) {
    if (opt.has_value()) {
        const unodb::value_view& vv = opt.value();
        std::string_view sv(reinterpret_cast<const char*>(vv.data()), vv.size());
        uint64_t ptr_val = std::stoull(std::string(sv));
        return reinterpret_cast<Vertex*>(ptr_val);
    }
    return nullptr;
}

void ART::InsertSimpleVertex(NodeID id) {
    unodb::quiescent_state_on_scope_exit qstate_on_exit{};
    tree.insert(id, from_string_view(test_value));
}

bool ART::CheckExistence(NodeID id) {
    unodb::quiescent_state_on_scope_exit qstate_on_exit{};
    auto result = tree.get(id);
    return result.has_value();
}

Vertex* ART::RetrieveVertex(NodeID id, bool insert_mode) {
    unodb::quiescent_state_on_scope_exit qstate_on_exit{};
    auto result = tree.get(id);
    if (result.has_value()) {
        return optional_value_view_to_vertex_ptr(result);
    } else if (insert_mode) {
        #if USE_VERTEX_TABLE
            // Grow vertex table and insert to ART
            mtx.lock();
            result = tree.get(id);
            if (result.has_value()) {
                mtx.unlock();
                return optional_value_view_to_vertex_ptr(result);
            }
            // Insert new vertex
            int i = -1;
            #if ENABLE_GARBAGE_COLLECTION
                if (deleted_slots.try_pop(i)) {
                    // Reuse a deleted slot
                    i = i;
                }
            #endif
            if (i == -1) i = cnt.fetch_add(1);
            auto tmp = &(*vertex_table.grow_by(1));
            tmp->node = id;
            tmp->idx = i;
            tmp->next.store(new WeightedEdgeArray(8));
            if (global_info.is_mixed_workloads) tmp->next.load()->timestamp = new int[8];
            auto insert_result = tree.insert(id, from_string_view(std::to_string((uint64_t)tmp)));
            mtx.unlock();
            return tmp;
        #else
            auto tmp = new Vertex();
            tmp->node = id;
            tmp->idx = id; // Use node ID as index directly
            tmp->next.store(new WeightedEdgeArray(8));
            if (global_info.is_mixed_workloads) tmp->next.load()->timestamp = new int[8];
            auto insert_result = tree.insert(id, from_string_view(std::to_string((uint64_t)tmp)));
            if (!insert_result) {
                delete tmp;
                auto result = tree.get(id);
                return optional_value_view_to_vertex_ptr(result);
            }
            else {
                return tmp;
            }
        #endif
    } else {
        return nullptr;
    }
}

bool ART::DeleteVertex(NodeID id) {
    unodb::quiescent_state_on_scope_exit qstate_on_exit{};
    Vertex* tmp = RetrieveVertex(id);
    if (tmp == nullptr) {
        return false;
    }
    // Do not delete the vertex directly; defer it to get_neighbor and log compaction.
    tmp->del_time = global_info.global_timestamp;
    #if ENABLE_GARBAGE_COLLECTION
        deleted_slots.push(tmp->idx);
    #endif
    return true;
}