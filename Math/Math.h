#pragma once

#include <glm/glm.hpp>
#include <glm/detail/type_quat.hpp>

#define FLOAT_SMALL_NUMBER (1.e-8f)

namespace Math {

static glm::vec3 RotateVectorByQuaternion(const glm::quat quaternion, const glm::vec3 vector) {
    const glm::quat quatPoint{vector.x, vector.y, vector.z, 0.f};

    const glm::quat partialQuatMultiply{quaternion * quatPoint};
    const glm::quat result{partialQuatMultiply * glm::inverse(quaternion)};

    return {result.x, result.y, result.z};
}

[[nodiscard]] static bool IsNearlyEqual(const float first, const float second, const float errorTolerance = FLOAT_SMALL_NUMBER) {
    return std::abs(first - second) <= errorTolerance;
}

[[nodiscard]] static bool IsGreaterOrEqual(const float first, const float second, const float errorTolerance = FLOAT_SMALL_NUMBER) {
    return first > second || std::abs(first - second) <= errorTolerance;
}

[[nodiscard]] static bool IsLesserOrEqual(const float first, const float second, const float errorTolerance = FLOAT_SMALL_NUMBER) {
    return first < second || std::abs(first - second) <= errorTolerance;
}

}