#include "Transform.h"

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