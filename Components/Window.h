#pragma once

#include <GLFW/glfw3.h>

#include <entt/entt.hpp>

struct Window {
    explicit Window(entt::registry& registry);

    GLFWwindow* GlfwWindow{nullptr};
    entt::registry& Registry;

    int Width{};
    int Height{};
};