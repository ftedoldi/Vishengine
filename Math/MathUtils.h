#pragma once

#include <glm/gtc/quaternion.hpp>
#include <glm/vec3.hpp>

namespace MathUtils{
    glm::quat EulerAnglesToQuaternion(glm::vec3 eulerAngles);
}