#pragma once

#include "DataStructures/Octree.h"
#include "Events/GameEvents.h"

#include <entt/entt.hpp>
#include <unordered_set>

class SpatialSystem {
public:
    SpatialSystem(Octree* octree, entt::registry& registry, entt::dispatcher& sceneDispatcher);

    void Init() const;

    void Update(entt::registry& registry);

private:
    void _onTransformUpdated(GameEvents::TransformUpdated transformUpdatedEvent);

    void _insertNewEntities(entt::registry& registry) const;

    void _updateOctree(entt::registry& registry) const;

    void _performFrustumCulling(entt::registry& registry) const;

    std::unordered_set<entt::entity> _entities{};

    Octree* _octree{};

    entt::registry& _registry;
};