#pragma once

#include "Math/Transform.h"
#include "entt/entity/registry.hpp"

class TransformSystem {
public:
    void Update(entt::registry& registry);

private:
    Transform _getOrComputeWorldTransform(entt::entity entity, entt::registry& registry);
};

