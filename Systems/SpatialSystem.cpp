#include "SpatialSystem.h"

#include "Components/OctreeLocation.h"
#include "DataStructures/Octree.h"

SpatialSystem::SpatialSystem(Octree* const octree, entt::registry& registry, entt::dispatcher& sceneDispatcher) : _octree{octree}, _registry{registry}, _sceneDispatcher{sceneDispatcher} {
    sceneDispatcher.sink<GameEvents::TransformUpdated>().connect<&SpatialSystem::_onTransformUpdated>(this);
    // The first time all the transforms are updated, the octree is initialized.
    sceneDispatcher.sink<GameEvents::AllTransformsUpdated>().connect<&SpatialSystem::_initOctree>(this);
}

void SpatialSystem::Update(entt::registry& registry) {
    _updateOctree(registry);
    _entities.clear();
}

void SpatialSystem::_onTransformUpdated(const GameEvents::TransformUpdated transformUpdatedEvent) {
    // TODO: use a custom allocator here
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

void SpatialSystem::_initOctree(const GameEvents::AllTransformsUpdated) {
    assert(_octree);
    constexpr int32_t maxDepth{6};
    _octree->Init(_registry, maxDepth);

    // After the octree is initialized, disconnect this function from the dispatcher.
    _sceneDispatcher.sink<GameEvents::AllTransformsUpdated>().disconnect<&SpatialSystem::_initOctree>(this);
}
