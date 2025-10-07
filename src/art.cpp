#include "art.h"

constexpr std::string_view test_value = "test";

[[nodiscard, gnu::pure]] unodb::value_view from_string_view(
    std::string_view sv) {
  return {reinterpret_cast<const std::byte *>(sv.data()), sv.length()};
}

// Funtion to convert std::optional<value_view> to Vertex*
Vertex* optional_value_view_to_vertex_ptr(const std::optional<unodb::value_view>& opt) {
    if (opt.has_value()) {
        const unodb::value_view& vv = opt.value();
        if (vv.size() == sizeof(Vertex)) {
            return reinterpret_cast<Vertex*>(const_cast<std::byte*>(vv.data()));
        }
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
        // Grow vertex table and insert to ART
        mtx.lock();
        result = tree.get(id);
        if (result.has_value()) {
            mtx.unlock();
            return optional_value_view_to_vertex_ptr(result);
        }
        // Insert new vertex
        int i = cnt.fetch_add(1);
        auto tmp = &(*vertex_table.grow_by(1));
        tmp->node = id;
        tmp->idx = i;
        tmp->next.store(new WeightedEdgeArray(8));
        if (global_info.is_mixed_workloads) tmp->next.load()->timestamp = new int[8];
        auto insert_result = tree.insert(id, unodb::value_view{(const std::byte*)tmp, sizeof(Vertex)});
        mtx.unlock();
        return tmp;
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
    return true;
}