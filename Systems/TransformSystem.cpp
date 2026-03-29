#include "TransformSystem.h"

#include "Components/BoundingSphere.h"
#include "Components/Relationship.h"
#include "Components/Transforms/RelativeTransform.h"
#include "Components/Transforms/TransformDirtyFlag.h"
#include "Components/Transforms/WorldTransform.h"

void TransformSystem::Update(entt::registry& registry) {
    // TODO: very slow
    auto view{registry.view<RelativeTransform, WorldTransform, TransformDirtyFlag>()};
    for (const auto entity: view) {
        auto& transformFlag{view.get<TransformDirtyFlag>(entity)};
        if (transformFlag.ShouldUpdateTransform) {
            const auto worldTransform{_getOrComputeWorldTransform(entity, registry)};
            auto& worldTransformComponent{registry.get<WorldTransform>(entity)};
            worldTransformComponent.Value = worldTransform;
            // Here I should probably fire an event about which entity had the world transform changed.
            // But for now I write all the code related to the transform change here.
            if (auto* const boundingSphere{registry.try_get<BoundingSphere>(entity)}) {
                boundingSphere->WorldCenter = worldTransform.TransformPosition(boundingSphere->LocalCenter);
                boundingSphere->WorldRadius = boundingSphere->LocalRadius * worldTransform.Scale;
            }

            transformFlag.ShouldUpdateTransform = false;
        }
    }
}

Transform TransformSystem::_getOrComputeWorldTransform(const entt::entity entity, entt::registry& registry) {
    const auto relativeTransform{registry.get<RelativeTransform>(entity).Value};

    auto finalTransform{relativeTransform};

    if (registry.all_of<Relationship>(entity)) {
        const auto& relationship{registry.get<Relationship>(entity)};

        if (relationship.Parent != entt::null) {
            const auto parentWorld{_getOrComputeWorldTransform(relationship.Parent, registry)};

            finalTransform = parentWorld.Cumulate(relativeTransform);
        }
    }

    return finalTransform;
}
