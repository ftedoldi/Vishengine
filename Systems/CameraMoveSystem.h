#pragma once

#include "Components/CameraComponents/Camera.h"
#include "Components/Transform.h"
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
    Transform* _cameraTransform;

    // TODO: change where this is put
    float _cameraYPosition{0.f};
};
