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
    InputManager* _inputManager;
    Camera* _camera;

    Position* _position;
    Rotation* _rotation;

    float _cameraYPosition{0.f};
};
