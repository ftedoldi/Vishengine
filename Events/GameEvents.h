#pragma once

#include <entt/entt.hpp>

namespace GameEvents {

    struct TransformUpdatedEvent {
        // The entity to which the transform was updated.
        entt::entity entity{};
    };

}
