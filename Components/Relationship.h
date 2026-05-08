#pragma once

#include <entt/entt.hpp>

// Relationship is used in two ways:
// 1. On MeshNode entities: full hierarchy (Parent + Children[]).
// 2. On Mesh entities: back-pointer to the owning MeshNode only (Children is unused; Mesh entities are leaves).
// TODO: this probably needs to be reviewed.
struct Relationship {
    entt::entity Parent{entt::null};

    // The current number of children.
    std::size_t Size{};

    static constexpr std::size_t MAX_NUM_OF_CHILDREN{10};
    std::array<entt::entity, MAX_NUM_OF_CHILDREN> Children{entt::null};
};