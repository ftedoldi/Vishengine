#pragma once

#include <GLFW/glfw3.h>

#include <functional>

struct KeyPressedEventComponent {
    std::function<void(GLFWwindow*, int, int, int, int)> OnKeyPressed;
};