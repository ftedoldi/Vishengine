#pragma once

#include "glm/glm.hpp"

#include <GLFW/glfw3.h>

class InputManager {
public:
    explicit InputManager(GLFWwindow* window);

    void Update();

    [[nodiscard]] bool IsKeyPressed(int key) const;

    [[nodiscard]] bool IsMouseButtonPressed(int button) const;

    [[nodiscard]] glm::vec2 GetMouseDelta() const;

    [[nodiscard]] float GetScrollDelta();

    static void ScrollCallback(GLFWwindow* window, double xOffset, double yOffset);

private:
    GLFWwindow* _mainWindow{};

    glm::dvec2 _lastMousePos{};

    glm::vec2 _mouseDelta{};

    float _scrollDelta{};
};