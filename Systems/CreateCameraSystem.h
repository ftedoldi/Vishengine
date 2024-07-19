#pragma once

#include "Components/CameraComponents/Camera.h"

#include <entt/entt.hpp>

class CreateCameraSystem {
public:
    explicit CreateCameraSystem(entt::registry& registry, entt::entity window);

    entt::entity CreateCamera(CameraType cameraType) const;

private:
    void _setupPerspective(entt::entity cameraEntity) const;

    //void _setupOrthogonal();

    entt::registry& _registry;
    entt::entity _window;
};