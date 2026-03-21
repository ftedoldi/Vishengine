#pragma once

#include "GLFW/glfw3.h"
#include <entt/entt.hpp>

class GUIDrawer {
public:
    explicit GUIDrawer(GLFWwindow* window);

    void StartFrame(entt::registry& registry);

    void Render();

    void Clear();
};
