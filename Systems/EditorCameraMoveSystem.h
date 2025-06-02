#pragma once

#include "Components/Camera/Camera.h"
#include "Components/Position.h"
#include "Components/Rotation.h"

#include "Core/InputManager.h"

#include <entt/entt.hpp>

class EditorCameraMoveSystem {
public:
    explicit EditorCameraMoveSystem(InputManager* inputManager);

    void Update(float deltaTime, entt::registry& registry);

private:
    InputManager* _inputManager{};

    float _cameraYPosition{0.f};
};
