#include "art.h"

constexpr std::string_view test_value = "test";

[[nodiscard, gnu::pure]] unodb::value_view from_string_view(
    std::string_view sv) {
  return {reinterpret_cast<const std::byte *>(sv.data()), sv.length()};
}

void ART::InsertSimpleVertex(NodeID id) {
    tree.insert(id, from_string_view(test_value));
}

bool ART::CheckExistence(NodeID id) {
    auto result = tree.get(id);
    return result.has_value();
}

Vertex* ART::RetrieveVertex(NodeID id, bool insert_mode) {
    auto result = tree.get(id);
    if (result.has_value()) {
        // Convert std::optional<value_view> to Vertex*
        // return (Vertex*)result->data();
        
    } else if (insert_mode) {
        // Grow vertex table and insert to ART
        int i = cnt.fetch_add(1);
        Vertex* v = &(*vertex_table.grow_by(1));
        v->node = id;
        v->idx = i;
        auto insert_result = tree.insert(id, unodb::value_view{(const std::byte*)v, sizeof(Vertex)});
        if (!insert_result) {
            // Another thread has inserted the vertex
            // return (Vertex*)tree.get(id).data();
        } else {
            return v;
        }
    } else {
        return nullptr;
    }
}

bool ART::DeleteVertex(NodeID id) {
    Vertex* tmp = RetrieveVertex(id);
    if (tmp == nullptr) {
        return false;
    }
    // Do not delete the vertex directly; defer it to get_neighbor and log compaction.
    tmp->del_time = global_info.global_timestamp;
    return true;
}