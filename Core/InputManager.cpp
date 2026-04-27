#include "InputManager.h"

#include <unordered_map>

static std::unordered_map<GLFWwindow*, InputManager*> s_inputManagers;

InputManager::InputManager(GLFWwindow* const window) : _mainWindow{window} {
    double x{}, y{};
    glfwGetCursorPos(_mainWindow, &x, &y);
    _lastMousePos = {x, y};

    s_inputManagers[_mainWindow] = this;
    glfwSetScrollCallback(_mainWindow, &InputManager::ScrollCallback);
}

void InputManager::Update() {
    double x{}, y{};
    glfwGetCursorPos(_mainWindow, &x, &y);
    _mouseDelta    = {static_cast<float>(x - _lastMousePos.x), static_cast<float>(y - _lastMousePos.y)};
    _lastMousePos  = {x, y};
}

bool InputManager::IsKeyPressed(const int key) const {
    return glfwGetKey(_mainWindow, key) == GLFW_PRESS;
}

bool InputManager::IsMouseButtonPressed(const int button) const {
    return glfwGetMouseButton(_mainWindow, button) == GLFW_PRESS;
}

glm::vec2 InputManager::GetMouseDelta() const {
    return _mouseDelta;
}

float InputManager::GetScrollDelta() {
    const float delta{_scrollDelta};
    _scrollDelta = 0.f;
    return delta;
}

void InputManager::ScrollCallback(GLFWwindow* window, double, const double yOffset) {
    if (const auto it{s_inputManagers.find(window)}; it != s_inputManagers.end()) {
        it->second->_scrollDelta += static_cast<float>(yOffset);
    }
}