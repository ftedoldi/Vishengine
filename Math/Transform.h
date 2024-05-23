#pragma once

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

class Transform {
public:
    Transform() = default;
    Transform(glm::vec3 translation, float scaling, glm::quat rotation);

    void SetTranslation(glm::vec3 translation);
    void SetScaling(float scaling);
    void SetRotation(glm::quat rotation);

    glm::vec3 ApplyToPoint(glm::vec3 point) const;

    Transform operator*(const Transform& otherTransform) const;

    Transform CumulateWith(const Transform& otherTransform) const;

    Transform Inverse() const;

    glm::vec3 Translation{};
    float Scaling{1.f};
    glm::quat Rotation{};
};