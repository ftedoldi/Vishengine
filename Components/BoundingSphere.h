#pragma once

#include "glm/vec3.hpp"

struct BoundingSphere {
    BoundingSphere(const glm::vec3 center, const float radius) : Center{center}, Radius{radius} {}

    glm::vec3 Center{};

    float Radius{};
};