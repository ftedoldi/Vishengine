#pragma once

#include <GLFW/glfw3.h>

#include <functional>

struct InputMovable {
    // The parameter represents the key that was pressed.
    std::function<void(int)> OnKeyPressed;
};