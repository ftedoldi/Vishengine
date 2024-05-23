#include "Transform.h"

Transform::Transform(const glm::vec3 translation, const float scaling, const glm::quat rotation) : Translation(translation), Scaling(scaling), Rotation(rotation) {}

void Transform::SetTranslation(const glm::vec3 translation) {
    Translation = translation;
}

void Transform::SetScaling(const float scaling) {
    Scaling = scaling;
}

void Transform::SetRotation(const glm::quat rotation) {
    Rotation = rotation;
}

glm::vec3 Transform::ApplyToPoint(const glm::vec3 point) const {
    return Rotation * (point * Scaling) + Translation;
}

Transform Transform::operator*(const Transform &otherTransform) const {
    return CumulateWith(otherTransform);
}

Transform Transform::CumulateWith(const Transform &otherTransform) const {
    Transform result;
    result.Scaling = Scaling * otherTransform.Scaling;
    result.Rotation = Rotation * otherTransform.Rotation;
    // check
    result.Translation = Rotation * (Translation * otherTransform.Scaling) + otherTransform.Translation;
    return result;
}

Transform Transform::Inverse() const {
    Transform transform;

    float scaling{1.f / Scaling};
    glm::quat rotation{glm::inverse(Rotation)};
    glm::vec3 translation = rotation * (-Translation * scaling);

    transform.Translation = translation;
    transform.Rotation = rotation;
    transform.Scaling = scaling;

    return transform;
}