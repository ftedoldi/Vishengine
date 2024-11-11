#pragma once

#include "glm/glm.hpp"

#include <entt/entt.hpp>

#include <vector>

class PhysicsSystem {
public:
    explicit PhysicsSystem(entt::registry& registry);

    void Update();

private:
    entt::registry& _registry;
};
