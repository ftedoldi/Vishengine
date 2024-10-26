#pragma once

#include <glm/glm.hpp>
#include <glm/detail/type_quat.hpp>

namespace Math {
    static glm::vec3 RotateVectorByQuaternion(const glm::quat quaternion, const glm::vec3 vector) {
        glm::quat quatPoint{vector.x, vector.y, vector.z, 0.f};

        glm::quat partialQuatMultiply{quaternion * quatPoint};
        glm::quat result{partialQuatMultiply * glm::inverse(quaternion)};

        return {result.x, result.y, result.z};
    }
}