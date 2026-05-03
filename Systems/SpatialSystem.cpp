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
#include "DataStructures/Plane.h"

#include <glm/gtc/matrix_access.hpp>

namespace {

    Plane ExtractPlane(const glm::vec4& row) {
        const float length{glm::length(glm::vec3{row.x, row.y, row.z})};
        return { glm::vec3{row.x, row.y, row.z} / length, row.w / length };
    }

}

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
    _performFrustumCulling(registry);
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
    auto activeCameraView{registry.view<Camera, ActiveCameraTag>()};

    activeCameraView.each([this, &registry](const entt::entity, const Camera& camera) {
        const auto clipMatrix{camera.ProjectionMatrix * camera.ViewTransform.ToMatrix()};

        const auto r0{glm::row(clipMatrix, 0)};
        const auto r1{glm::row(clipMatrix, 1)};
        const auto r2{glm::row(clipMatrix, 2)};
        const auto r3{glm::row(clipMatrix, 3)};

        const Frustum frustum{std::array{
            ExtractPlane(r0 + r3),  // left
            ExtractPlane(r3 - r0),  // right
            ExtractPlane(r1 + r3),  // bottom
            ExtractPlane(r3 - r1),  // top
            ExtractPlane(r2 + r3),  // near
            ExtractPlane(r3 - r2),  // far
        }};

        // Clear renderable tags from all mesh entities before rebuilding.
        for (const auto entity : registry.view<MeshNodeTag>()) {
            registry.remove<RenderableTag>(entity);
        }

        // Use octree to skip entire subtrees outside the frustum, then test
        // each surviving entity's world-space AABB for a precise accept/reject.
        for (const auto entity : _octree->QueryFrustum(frustum)) {
            if (!registry.all_of<BoundingBox, WorldTransform, MeshNodeTag>(entity)) {
                continue;
            }
            if (const auto worldSpaceBox{CoordUtils::ComputeWorldSpaceBox(entity, registry)};
                FrustumUtils::IsAABBInsideFrustum(worldSpaceBox, frustum)) {
                registry.emplace<RenderableTag>(entity);
            }
        }
    });
}