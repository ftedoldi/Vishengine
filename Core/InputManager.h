#pragma once

#include <GLFW/glfw3.h>

class InputManager {
public:
    explicit InputManager(GLFWwindow* window);

    bool IsKeyPressed(int key);

private:
    GLFWwindow* _mainWindow;
};
