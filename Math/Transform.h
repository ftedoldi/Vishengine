#pragma once

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

class Transform {
public:
    Transform() = default;
    Transform(glm::vec3 translation, float scaling, glm::quat rotation);

    glm::vec3 ApplyToPoint(glm::vec3 point) const;

    Transform operator*(const Transform& otherTransform) const;

    Transform CumulateWith(const Transform& otherTransform) const;

    Transform Inverse() const;

    void SetTranslation(glm::vec3 translation);
    void SetScaling(float scaling);
    void SetRotation(glm::quat rotation);

    glm::vec3 GetTranslation() const;
    float GetScaling() const;
    glm::quat GetRotation() const;

private:
    glm::vec3 _translation{};
    float _scaling{1.f};
    glm::quat _rotation{0.0, 0.0, 0.0, 1.0};
};