#pragma once

#include <entt/entt.hpp>

namespace GameEvents {

struct TransformUpdated {
    // The entity to which the transform was updated.
    entt::entity entity{};
};

struct AllTransformsUpdated {};

}
