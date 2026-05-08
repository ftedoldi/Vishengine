#include "SpatialSystem.h"

#include "Components/BoundingBox.h"
#include "Components/Camera/ActiveCameraTag.h"
#include "Components/Camera/Camera.h"
#include "Components/MeshNodeTag.h"
#include "Components/OctreeLocation.h"
#include "Components/RenderableTag.h"
#include "Components/Transforms/WorldTransform.h"
#include "Coordinates/CoordinateUtils.h"
#include "DataStructures/Frustum.h"
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

void SpatialSystem::_performFrustumCulling(entt::registry& registry) const {
    const auto activeCameraView{registry.view<Camera, ActiveCameraTag>()};

    activeCameraView.each([this, &registry](const entt::entity, const Camera& camera) {
        const auto& viewFrustum{camera.ViewFrustum};

        // Clear renderable tags from all mesh entities before rebuilding.
        for (const auto entity : registry.view<MeshNodeTag>()) {
            registry.remove<RenderableTag>(entity);
        }

        // Use octree to skip entire subtrees outside the frustum, then test
        // each surviving entity's world-space AABB for a precise accept/reject.
        for (const auto entity : _octree->QueryFrustum(viewFrustum)) {
            if (!registry.all_of<BoundingBox, WorldTransform, MeshNodeTag>(entity)) {
                continue;
            }
            if (const auto worldSpaceBox{CoordUtils::ComputeWorldSpaceBox(entity, registry)};
                FrustumUtils::IsAABBInsideFrustum(worldSpaceBox, viewFrustum)) {
                registry.emplace<RenderableTag>(entity);
            }
        }
    });
}