#pragma once

#include <GLFW/glfw3.h>

#include <functional>

struct MouseMovedEventComponent {
    std::function<void(GLFWwindow*, double, double)> OnMouseMoved;
};