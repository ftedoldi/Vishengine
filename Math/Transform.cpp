#include "Transform.h"

Transform::Transform(const glm::vec3 translation, const float scaling, const glm::quat rotation) :
    _translation(translation), _scaling(scaling), _rotation(rotation) {}

glm::vec3 Transform::ApplyToPoint(const glm::vec3 point) {
    return _rotation * (point * _scaling) + _translation;
}
