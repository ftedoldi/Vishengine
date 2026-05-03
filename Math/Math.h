#pragma once

#include <glm/glm.hpp>
#include <glm/detail/type_quat.hpp>

namespace Math {

static constexpr float FLOAT_SMALL_NUMBER{1.e-8f};

[[nodiscard]] static bool IsNearlyEqual(const float first, const float second, const float errorTolerance = FLOAT_SMALL_NUMBER) {
    return std::abs(first - second) <= errorTolerance;
}

[[nodiscard]] static bool IsGreaterOrEqual(const float first, const float second, const float errorTolerance = FLOAT_SMALL_NUMBER) {
    return first > second || IsNearlyEqual(first, second, errorTolerance);
}

[[nodiscard]] static bool IsLesserOrEqual(const float first, const float second, const float errorTolerance = FLOAT_SMALL_NUMBER) {
    return first < second || IsNearlyEqual(first, second, errorTolerance);
}

}