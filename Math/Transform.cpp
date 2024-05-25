#include "Transform.h"

Transform::Transform(const glm::vec3 translation, const float scaling, const glm::quat rotation) : _translation(translation), _scaling(scaling), _rotation(rotation) {}

void Transform::SetTranslation(const glm::vec3 translation) {
    _translation = translation;
}

void Transform::SetScaling(const float scaling) {
    _scaling = scaling;
}

void Transform::SetRotation(const glm::quat rotation) {
    _rotation = rotation;
}

glm::vec3 Transform::ApplyToPoint(const glm::vec3 point) const {
    return _rotation * (point * _scaling) + _translation;
}

Transform Transform::operator*(const Transform &otherTransform) const {
    return CumulateWith(otherTransform);
}

Transform Transform::CumulateWith(const Transform &otherTransform) const {
    Transform result;

    const auto scaling{_scaling * otherTransform.GetScaling()};
    const auto rotation{_rotation * otherTransform.GetRotation()};
    const auto translation{otherTransform.GetRotation() * (_translation * otherTransform.GetScaling()) + otherTransform.GetTranslation()};

    result.SetScaling(scaling);
    result.SetRotation(rotation);
    result.SetTranslation(translation);

    return result;
}

Transform Transform::Inverse() const {
    Transform transform;

    const auto scaling{1.f / _scaling};
    const auto rotation{glm::inverse(_rotation)};
    const auto translation{rotation * (-_translation * scaling)};

    transform.SetTranslation(translation);
    transform.SetRotation(rotation);
    transform.SetScaling(scaling);

    return transform;
}

glm::vec3 Transform::GetTranslation() const {
    return _translation;
}

float Transform::GetScaling() const {
    return _scaling;
}

glm::quat Transform::GetRotation() const {
    return _rotation;
}
