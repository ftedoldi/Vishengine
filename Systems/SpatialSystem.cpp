#include "SpatialSystem.h"

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
    _entities.push_back(transformUpdatedEvent.entity);
}

void SpatialSystem::_updateOctree(entt::registry& registry) const {
    for (const auto entity : _entities) {
        //Octree::Update(entity, registry);
    }
}
