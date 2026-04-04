#include "TransformSystem.h"

#include "Components/Relationship.h"
#include "Components/Transforms/RelativeTransform.h"
#include "Components/Transforms/TransformDirtyFlag.h"
#include "Components/Transforms/WorldTransform.h"
#include "Events/GameEvents.h"

namespace {

Transform GetOrComputeWorldTransform(const entt::entity entity, entt::registry& registry) {
    const auto relativeTransform{registry.get<RelativeTransform>(entity).Value};
    auto finalTransform{relativeTransform};

    if (registry.all_of<Relationship>(entity)) {
        if (const auto parentEntity{registry.get<Relationship>(entity).Parent}; parentEntity != entt::null) {
            const auto parentWorld{GetOrComputeWorldTransform(parentEntity, registry)};
            finalTransform = parentWorld.Cumulate(relativeTransform);
        }
    }

    return finalTransform;
}

}

TransformSystem::TransformSystem(entt::dispatcher& eventDispatcher) : _eventDispatcher{eventDispatcher} {
}

void TransformSystem::Update(entt::registry& registry) const {
    const auto view{registry.view<RelativeTransform, WorldTransform, TransformDirtyFlag, Relationship>()};
    for (const auto entity: view) {
        if (auto& shouldUpdateTransform{view.get<TransformDirtyFlag>(entity).ShouldUpdateTransform}) {
            // Update the transforms of the current and child entities.
            _updateTransform(entity, registry);
            shouldUpdateTransform = false;
        }
    }
}

void TransformSystem::_updateTransform(const entt::entity entity, entt::registry& registry) const {
    const auto& relationship{registry.get<Relationship>(entity)};

    const auto worldTransform{GetOrComputeWorldTransform(entity, registry)};
    auto& worldTransformComponent{registry.get<WorldTransform>(entity).Value};
    worldTransformComponent = worldTransform;
    _eventDispatcher.trigger<GameEvents::TransformUpdatedEvent>({entity});

    for (uint32_t i{0}; i < relationship.Size; ++i) {
        _updateTransform(relationship.Children[i], registry);
    }
}
