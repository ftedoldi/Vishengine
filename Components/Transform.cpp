#include "Components/Transform.h"

Transform::Transform(const glm::vec3 translation, const float scaling, const glm::quat rotation) : Translation(translation), Scaling(scaling), Rotation(rotation) {}

Transform Transform::operator*(const Transform &otherTransform) const {
    return CumulateWith(otherTransform);
}

Transform Transform::CumulateWith(const Transform &otherTransform) const {
    Transform result;

    const auto scaling{Scaling * otherTransform.Scaling};
    const auto rotation{Rotation * otherTransform.Rotation};
    const auto translation{otherTransform.Rotation * (Translation * otherTransform.Scaling) + otherTransform.Translation};

    result.Scaling = scaling;
    result.Rotation = rotation;
    result.Translation = translation;

    return result;
}

Transform Transform::Inverse() const {
    Transform transform;

    const auto scaling{1.f / Scaling};
    const auto rotation{glm::inverse(Rotation)};
    const auto translation{rotation * (-Translation * scaling)};

    transform.Translation = translation;
    transform.Rotation = rotation;
    transform.Scaling = scaling;

    return transform;
}