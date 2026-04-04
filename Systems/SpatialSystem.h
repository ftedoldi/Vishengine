#pragma once

#include "Events/GameEvents.h"

#include <entt/entt.hpp>
#include <unordered_set>

class SpatialSystem {
public:
    SpatialSystem(entt::dispatcher& sceneDispatcher);

    void Update(entt::registry& registry);

private:
    void _onTransformUpdated(GameEvents::TransformUpdatedEvent transformUpdatedEvent);

    void _updateOctree(entt::registry& registry) const;

    std::unordered_set<entt::entity> _entities{};
};
