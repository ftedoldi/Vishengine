#include "SpatialSystem.h"

#include "Components/OctreeLocation.h"
#include "DataStructures/Octree.h"

SpatialSystem::SpatialSystem(entt::dispatcher& sceneDispatcher) {
    sceneDispatcher.sink<GameEvents::TransformUpdatedEvent>().connect<&SpatialSystem::_onTransformUpdated>(this);
}

void SpatialSystem::Update(entt::registry& registry) {
    _updateOctree(registry);
    _entities.clear();
}

void SpatialSystem::_onTransformUpdated(const GameEvents::TransformUpdatedEvent transformUpdatedEvent) {
    // TODO: use a custom allocator here
    _entities.insert(transformUpdatedEvent.entity);
}

void SpatialSystem::_updateOctree(entt::registry& registry) const {
    for (const auto entity : _entities) {
        if (registry.try_get<OctreeLocation>(entity)) {
            Octree::Update(entity, registry);
        }
    }
}
