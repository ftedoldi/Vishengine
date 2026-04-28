#include "SpatialSystem.h"

#include "Components/OctreeLocation.h"
#include "DataStructures/Octree.h"

SpatialSystem::SpatialSystem(Octree* const octree, entt::registry& registry, entt::dispatcher& sceneDispatcher)
    : _octree{octree}, _registry{registry} {
    sceneDispatcher.sink<GameEvents::TransformUpdated>().connect<&SpatialSystem::_onTransformUpdated>(this);
}

void SpatialSystem::Init() const {
    assert(_octree);
    constexpr int32_t maxDepth{6};
    _octree->Init(_registry, maxDepth);
}

void SpatialSystem::Update(entt::registry& registry) {
    _updateOctree(registry);
    _entities.clear();
}

void SpatialSystem::_onTransformUpdated(const GameEvents::TransformUpdated transformUpdatedEvent) {
    _entities.insert(transformUpdatedEvent.entity);
}

void SpatialSystem::_updateOctree(entt::registry& registry) const {
    assert(_octree);
    for (const auto entity : _entities) {
        if (registry.try_get<OctreeLocation>(entity)) {
            _octree->Update(entity, registry);
        }
    }
}