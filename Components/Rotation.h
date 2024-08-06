#pragma once

#include "glm/gtc/quaternion.hpp"

struct Rotation {
    Rotation() = default;
    explicit Rotation(glm::quat rotation) : Quaternion{rotation} {}

    glm::quat Quaternion{0.f, 0.f, 0.f, 1.f};
};