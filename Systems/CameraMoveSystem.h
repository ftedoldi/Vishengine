#pragma once

#include "Components/CameraComponents/Camera.h"
#include "Components/Position.h"
#include "Components/Rotation.h"

#include "InputManager/InputManager.h"

#include <entt/entt.hpp>

class CameraMoveSystem {
public:
    CameraMoveSystem(entt::registry& registry, entt::entity camera, InputManager* inputManager);

    void Update(float deltaTime);

private:
    entt::registry& _registry;

    InputManager* _inputManager{};

    entt::entity _camera{};

    float _cameraYPosition{0.f};
};
