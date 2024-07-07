#pragma once

#include <entt/entt.hpp>

class CreateMeshSystem {
public:
    explicit CreateMeshSystem(entt::registry& registry);

    void CreateMesh();

private:
    entt::registry& _registry;
};
