#pragma once

#include "Core/Window.h"

#include "Libs/entt/src/entt/entt.hpp"

class InputManager {
public:
    explicit InputManager(GLFWwindow* window);

    bool IsKeyPressed(int key);

private:
    GLFWwindow* _mainWindow;

    entt::registry _registry;
};
