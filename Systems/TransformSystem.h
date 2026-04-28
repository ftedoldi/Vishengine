#pragma once

#include "Math/Transform.h"
#include "entt/entity/registry.hpp"

class TransformSystem {
public:
    TransformSystem(entt::dispatcher& eventDispatcher);

    void Init(entt::registry& registry) const;

    void Update(entt::registry& registry) const;

private:
    void _initTransform(entt::entity entity, entt::registry& registry) const;

    void _updateTransform(entt::entity entity, entt::registry& registry) const;

    entt::dispatcher& _eventDispatcher;
};

