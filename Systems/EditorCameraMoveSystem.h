#pragma once

#include "Core/InputManager.h"

#include <entt/entt.hpp>

class EditorCameraMoveSystem {
public:
    explicit EditorCameraMoveSystem(const std::shared_ptr<InputManager>& inputManager);

    void Update(float deltaTime, entt::registry& registry);

private:
    std::shared_ptr<InputManager> _inputManager{};

    float _cameraYPosition{0.f};
};
