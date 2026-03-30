#include "SpatialSystem.h"

#include "Components/BoundingSphere.h"
#include "Components/Transforms/WorldTransform.h"

SpatialSystem::SpatialSystem(entt::dispatcher& sceneDispatcher) {
    sceneDispatcher.sink<GameEvents::TransformUpdatedEvent>().connect<&SpatialSystem::_onTransformUpdated>(this);
}

void SpatialSystem::Update(entt::registry& registry) {
    _updateBoundingVolumes(registry);
    _updateOctree();
    _entities.clear();
}

void SpatialSystem::_onTransformUpdated(const GameEvents::TransformUpdatedEvent transformUpdatedEvent) {
    // TODO: use a custom allocator here
    _entities.push_back(transformUpdatedEvent.entity);
}

void SpatialSystem::_updateBoundingVolumes(entt::registry& registry) const {
    for (const auto entity : _entities) {
        // Recompute spheres.
        if (auto* const boundingSphere{registry.try_get<BoundingSphere>(entity)}) {
            const auto& worldTransform{registry.get<WorldTransform>(entity).Value};
            boundingSphere->WorldCenter = worldTransform.TransformPosition(boundingSphere->LocalCenter);
            boundingSphere->WorldRadius = boundingSphere->LocalRadius * worldTransform.Scale;
        }
    }
}

void SpatialSystem::_updateOctree() const {
    for (const auto entity : _entities) {
        // Recompute octree.
    }
}
