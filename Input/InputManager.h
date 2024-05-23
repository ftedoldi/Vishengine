#pragma once

#include "GLFW/glfw3.h"

class InputManager {
public:
    InputManager(GLFWwindow* window) : _window(window) {}

    bool isKeyPressed(int key) {
        return glfwGetKey(_window, key) == GLFW_PRESS;
    }

private:
    GLFWwindow* _window;
};
