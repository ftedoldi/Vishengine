#pragma once

#include <GLFW/glfw3.h>

#include <functional>

struct FramebufferSizeEventComponent {
    std::function<void(GLFWwindow*, int, int)> OnFramebufferSizeChanged;
};