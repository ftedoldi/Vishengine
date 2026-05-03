#include "Transform.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

Transform::Transform(const glm::vec3 position, const glm::quat rotation, const float scale) :
           Position{position}, Rotation{rotation}, Scale{scale} {}

Transform Transform::Cumulate(const Transform& otherTransform) const {
    const auto position{Rotation * (otherTransform.Position * Scale) + Position};
    const auto rotation{Rotation * otherTransform.Rotation};
    const auto scale{Scale * otherTransform.Scale};

    return Transform{position, rotation, scale};
}

Transform Transform::Invert() const {
    const auto scale{1.f / Scale};
    const auto rotation{glm::inverse(Rotation)};
    const auto position{rotation * (-Position * scale)};

    return Transform{position, rotation, scale};
}

glm::vec3 Transform::TransformPosition(const glm::vec3 position) const {
    // 1. Scale (Multiply components)
    const glm::vec3 scaledPoint{position.x * Scale,
        position.y * Scale,
        position.z * Scale};

    // 2. Rotate (Multiply the Quaternion by the Vector3)
    const glm::vec3 rotatedPoint{Rotation * scaledPoint};

    // 3. Translate (Add the position)
    const glm::vec3 finalPoint{rotatedPoint + Position};

    return finalPoint;
}

glm::mat4 Transform::ToMatrix() const {
    const auto translation{glm::translate(glm::mat4{1.f}, Position)};
    const auto rotation{glm::mat4_cast(Rotation)};
    const auto scale{glm::scale(glm::mat4{1.f}, glm::vec3{Scale})};

    return translation * rotation * scale;
}