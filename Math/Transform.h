#pragma once

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

class Transform {
public:
    Transform(glm::vec3 translation, float scaling, glm::quat rotation);

    glm::vec3 ApplyToPoint(glm::vec3 point);

private:
    glm::vec3 _translation;
    float _scaling;
    glm::quat _rotation;
};