#pragma once

#include <GLFW/glfw3.h>

class InputManager {
public:
    explicit InputManager(GLFWwindow* window);

    [[nodiscard]] bool IsKeyPressed(int key) const;

private:
    GLFWwindow* _mainWindow{};
};
