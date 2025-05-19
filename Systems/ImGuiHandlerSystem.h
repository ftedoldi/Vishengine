#pragma once

#include "GLFW/glfw3.h"
#include <entt/entt.hpp>

class ImGuiHandlerSystem {
public:
    explicit ImGuiHandlerSystem(GLFWwindow* window);

    void StartFrame(entt::registry& registry);

    void Render();

    void Clear();
};
