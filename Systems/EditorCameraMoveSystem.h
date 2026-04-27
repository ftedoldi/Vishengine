#pragma once

#include "Core/InputManager.h"

#include <entt/entt.hpp>

class EditorCameraMoveSystem {
public:
    explicit EditorCameraMoveSystem(const std::shared_ptr<InputManager>& inputManager);

    void Update(float deltaTime, entt::registry& registry) const;

private:
    std::shared_ptr<InputManager> _inputManager{};
};