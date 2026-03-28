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
                boundingSphere->Center = worldTransform.TransformPosition(boundingSphere->Center);
            }

            transformFlag.ShouldUpdateTransform = false;
        }
    }
}

Transform TransformSystem::_getOrComputeWorldTransform(const entt::entity entity, entt::registry& registry) {
    const auto relativeTransform{registry.get<RelativeTransform>(entity)};

    Transform finalTransform = relativeTransform.Value;

    if (registry.all_of<Relationship>(entity)) {
        const auto& rel = registry.get<Relationship>(entity);

        if (rel.parent != entt::null) {
            Transform parentWorld = _getOrComputeWorldTransform(rel.parent, registry);

            finalTransform = parentWorld.Cumulate(relativeTransform.Value);
        }
    }

    return finalTransform;
}
