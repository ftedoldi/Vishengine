#include "InputManager.h"

InputManager::InputManager(GLFWwindow* const window) : _mainWindow{window} {
}

bool InputManager::IsKeyPressed(const int key) const {
    return glfwGetKey(_mainWindow, key) == GLFW_PRESS;
}
