#include "TransformSystem.h"

#include "Components/BoundingBox.h"
#include "Components/MeshNode.h"
#include "Components/Relationship.h"
#include "Components/Transforms/RelativeTransform.h"
#include "Components/Transforms/TransformDirtyFlag.h"
#include "Components/Transforms/WorldTransform.h"
#include "Components/WorldBoundingBox.h"
#include "Coordinates/CoordinateUtils.h"
#include "Coordinates/Transform.h"
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

void TransformSystem::Init(entt::registry& registry) const {
    const auto view{registry.view<RelativeTransform, WorldTransform, Relationship>()};
    for (const auto entity : view) {
        if (registry.get<Relationship>(entity).Parent == entt::null) {
            _initTransform(entity, registry);
        }
    }
}

void TransformSystem::Update(entt::registry& registry) const {
    const auto view{registry.view<RelativeTransform, WorldTransform, TransformDirtyFlag, Relationship>()};
    for (const auto entity: view) {
        _updateTransform(entity, registry);
        registry.remove<TransformDirtyFlag>(entity);
    }
}

void TransformSystem::_initTransform(const entt::entity entity, entt::registry& registry) const {
    const auto& relationship{registry.get<Relationship>(entity)};
    registry.get<WorldTransform>(entity).Value = GetOrComputeWorldTransform(entity, registry);

    if (registry.all_of<BoundingBox>(entity)) {
        registry.emplace_or_replace<WorldBoundingBox>(entity,
            CoordUtils::ComputeWorldSpaceBox(entity, registry));
    }

    for (uint32_t i{0}; i < relationship.Size; ++i) {
        _initTransform(relationship.Children[i], registry);
    }
}

void TransformSystem::_updateTransform(const entt::entity entity, entt::registry& registry) const {
    const auto& relationship{registry.get<Relationship>(entity)};
    const auto worldTransform{GetOrComputeWorldTransform(entity, registry)};

    // TODO: profile this shit
    if (auto* const meshNode{registry.try_get<MeshNode>(entity)}) {
        for (const auto meshEntity : meshNode->Meshes) {
            registry.get<WorldTransform>(meshEntity).Value = worldTransform;
        }
    }

    registry.get<WorldTransform>(entity).Value = worldTransform;

    if (auto* wbb{registry.try_get<WorldBoundingBox>(entity)}) {
        wbb->Box = CoordUtils::ComputeWorldSpaceBox(entity, registry);
    }

    _eventDispatcher.trigger<GameEvents::TransformUpdated>({entity});

    for (uint32_t i{0}; i < relationship.Size; ++i) {
        _updateTransform(relationship.Children[i], registry);
    }
}