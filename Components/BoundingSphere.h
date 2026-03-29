#pragma once

#include "glm/vec3.hpp"

struct BoundingSphere {
    BoundingSphere(const glm::vec3 localCenter, const float localRadius)
        : LocalCenter{localCenter}
        , LocalRadius{localRadius}
        , WorldCenter{localCenter}
        , WorldRadius{localRadius} {}

    glm::vec3 LocalCenter{};

    float LocalRadius{};

    glm::vec3 WorldCenter{};

    float WorldRadius{};
};
