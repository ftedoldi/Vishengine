#pragma once

#include <entt/entt.hpp>
#include <Components/Window.h>

class InputManager {
public:
    explicit InputManager(entt::registry& registry, entt::entity mainWindow);

    bool IsKeyPressed(int key);

private:
    Window* _mainWindow;

    entt::registry _registry;
};
