#pragma once

#include "glm/gtc/quaternion.hpp"
#include "glm/vec3.hpp"

struct Transform {
    Transform() = default;
    Transform(glm::vec3 translation, float scaling, glm::quat rotation);

    Transform operator*(const Transform& otherTransform) const;

    Transform CumulateWith(const Transform& otherTransform) const;

    Transform Inverse() const;

    glm::vec3 Translation{};
    float Scaling{1.f};
    glm::quat Rotation{0.0, 0.0, 0.0, 1.0};
};