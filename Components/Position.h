#pragma once

#include <glm/vec3.hpp>

struct Position {
    Position() = default;
    explicit Position(glm::vec3 position) : Vector{position} {}

    glm::vec3 Vector{};
};