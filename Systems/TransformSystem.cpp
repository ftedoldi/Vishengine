#include "TransformSystem.h"

#include "Components/Position.h"
#include "Components/Relationship.h"
#include "Components/Rotation.h"
#include "Components/Scale.h"
#include "Components/WorldTransform.h"

void TransformSystem::Update(entt::registry& registry) {
    // Clear all the world transforms calculated the previous frame.
    registry.clear<WorldTransform>();

    auto view{registry.view<Position, Rotation, Scale>()};
    for (const auto entity: view) {
        _getOrComputeWorldTransform(entity, registry);
    }
}

Transform TransformSystem::_getOrComputeWorldTransform(const entt::entity entity, entt::registry& registry) {
    // --- THE CACHE CHECK ---
    // If we already calculated this entity's WorldTransform this frame,
    // return it immediately! This prevents doing the math twice.
    if (const auto* const cachedWorldTransform = registry.try_get<WorldTransform>(entity)) {
        return cachedWorldTransform->Value;
    }

    Transform localTransform{{0.f, 0.f, 0.f}};
    if (registry.all_of<Position, Rotation, Scale>(entity)) {
        const auto position = registry.get<Position>(entity);
        const auto& rotation = registry.get<Rotation>(entity);
        const auto scale = registry.get<Scale>(entity);
        localTransform = Transform{position.Vector, rotation.Quaternion, scale.Value};
    }

    Transform finalTransform = localTransform;

    // Check if it has a Parent
    if (registry.all_of<Relationship>(entity)) {
        const auto& rel = registry.get<Relationship>(entity);

        if (rel.parent != entt::null) {
            // RECURSE UP: Get the parent's generated WorldTransform.
            // Because of the cache check above, the parent will only calculate
            // itself once, even if 100 children ask for it!
            Transform parentWorld = _getOrComputeWorldTransform(rel.parent, registry);

            finalTransform = parentWorld.Cumulate(localTransform);
        }
    }

    // --- STORE THE CACHE ---
    // Save it to the registry so any siblings asking for this parent
    // will get it for free, and the Renderer can grab it instantly.
    registry.emplace<WorldTransform>(entity, finalTransform);

    return finalTransform;
}
