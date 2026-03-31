#pragma once

#include <entt/entt.hpp>

struct Relationship {
private:
    static constexpr std::size_t MAX_NUM_OF_CHILDREN{10};

public:
    entt::entity Parent{entt::null};

    // The current number of children.
    std::size_t Size{};

    std::array<entt::entity, MAX_NUM_OF_CHILDREN> Children{entt::null};
};