#include "SpatialSystem.h"

#include "Components/BoundingBox.h"
#include "Components/Camera/ActiveCameraTag.h"
#include "Components/Camera/Camera.h"
#include "Components/MeshNode.h"
#include "Components/OctreeLocation.h"
#include "Components/RenderableTag.h"
#include "Components/Transforms/WorldTransform.h"
#include "Components/WorldBoundingBox.h"
#include "DataStructures/Frustum.h"
#include "DataStructures/Octree.h"
#include "ScopedTimer.h"

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
    _insertNewEntities(registry);
    _updateOctree(registry);
    _entities.clear();
    _performFrustumCulling(registry);
}

void SpatialSystem::_insertNewEntities(entt::registry& registry) const {
    if (!_octree->GetRootNode()) {
        return;
    }
    const auto newEntitiesView{registry.view<BoundingBox, WorldTransform>(entt::exclude<OctreeLocation>)};
    for (const auto entity : newEntitiesView) {
        registry.emplace<OctreeLocation>(entity);
        _octree->InsertEntity(_octree->GetRootNode(), entity, registry);
    }
}

void SpatialSystem::_onTransformUpdated(const GameEvents::TransformUpdated transformUpdatedEvent) {
    _entities.insert(transformUpdatedEvent.Entity);
}

void SpatialSystem::_updateOctree(entt::registry& registry) const {
    assert(_octree);
    for (const auto entity : _entities) {
        if (registry.try_get<OctreeLocation>(entity)) {
            _octree->Update(entity, registry);
        }
    }
}

void SpatialSystem::_performFrustumCulling(entt::registry& registry) const {
    const auto activeCameraView{registry.view<Camera, ActiveCameraTag>()};

    activeCameraView.each([this, &registry](const Camera& camera) {
        const auto& viewFrustum{camera.ViewFrustum};
        registry.clear<RenderableTag>();

        // Use octree to skip entire subtrees outside the frustum, then test
        // each surviving entity's world-space AABB for a precise accept/reject.
        const auto entities{_octree->QueryFrustum(viewFrustum)};
        {
            PROFILE_SCOPE("INTERSECT FRUSTUM");
            for (const auto entity : entities) {
                // TODO: this should be removed, the entities here SHOULD have a bounding box. I should add worldBBox to point lights
               if (!registry.all_of<WorldBoundingBox>(entity)) {
                   continue;
               }
                // TODO: is this for real the best way?
               const auto& worldBBox{registry.get<WorldBoundingBox>(entity)};
               if (FrustumUtils::IntersectsFrustum(worldBBox.Box, viewFrustum)) {
                   if (auto* const meshNode{registry.try_get<MeshNode>(entity)}) {
                    for (const auto meshEntity : meshNode->Meshes) {
                        registry.emplace<RenderableTag>(meshEntity);
                    }
                }
               registry.emplace<RenderableTag>(entity);
               }
            }
        }
    });
}