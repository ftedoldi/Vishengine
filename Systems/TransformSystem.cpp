#include "TransformSystem.h"

#include "Components/BoundingSphere.h"
#include "Components/Relationship.h"
#include "Components/Transforms/RelativeTransform.h"
#include "Components/Transforms/TransformDirtyFlag.h"
#include "Components/Transforms/WorldTransform.h"

TransformSystem::TransformSystem(Octree::Node* octreeRootNode) : _octreeRootNode{octreeRootNode} {
}

void TransformSystem::Update(entt::registry& registry) {
    // TODO: very slow
    auto view{registry.view<RelativeTransform, WorldTransform, TransformDirtyFlag>()};
    for (const auto entity: view) {
        if (auto& shouldUpdateTransform{view.get<TransformDirtyFlag>(entity).ShouldUpdateTransform}) {
            const auto worldTransform{_getOrComputeWorldTransform(entity, registry)};
            auto& worldTransformComponent{registry.get<WorldTransform>(entity).Value};
            worldTransformComponent = worldTransform;
            // Here I should probably fire an event about which entity had the world transform changed.
            // But for now I write all the code related to the transform change here.
            if (auto* const boundingSphere{registry.try_get<BoundingSphere>(entity)}) {
                boundingSphere->WorldCenter = worldTransform.TransformPosition(boundingSphere->LocalCenter);
                boundingSphere->WorldRadius = boundingSphere->LocalRadius * worldTransform.Scale;
            }

            // TODO: Update the octree based on the modified entities.

            shouldUpdateTransform = false;
        }
    }
}

Transform TransformSystem::_getOrComputeWorldTransform(const entt::entity entity, entt::registry& registry) {
    const auto relativeTransform{registry.get<RelativeTransform>(entity).Value};
    auto finalTransform{relativeTransform};

    if (registry.all_of<Relationship>(entity)) {
        if (const auto parentEntity{registry.get<Relationship>(entity).Parent}; parentEntity != entt::null) {
            const auto parentWorld{_getOrComputeWorldTransform(parentEntity, registry)};
            finalTransform = parentWorld.Cumulate(relativeTransform);
        }
    }

    return finalTransform;
}
