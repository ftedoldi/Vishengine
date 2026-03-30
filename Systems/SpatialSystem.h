#pragma once

#include "Events/GameEvents.h"

#include <entt/entt.hpp>

class SpatialSystem {
public:
    SpatialSystem(entt::dispatcher& sceneDispatcher);

    void Update(entt::registry& registry);

private:
    void _onTransformUpdated(GameEvents::TransformUpdatedEvent transformUpdatedEvent);

    void _updateBoundingVolumes(entt::registry& registry) const;

    void _updateOctree() const;

    std::vector<entt::entity> _entities{};
};