#pragma once
#include <cmath>

namespace Math {

static constexpr float FLOAT_SMALL_NUMBER{1.e-8f};

[[nodiscard]] inline bool IsNearlyEqual(const float first, const float second, const float errorTolerance = FLOAT_SMALL_NUMBER) {
    return std::abs(first - second) <= errorTolerance;
}

[[nodiscard]] inline bool IsGreaterOrEqual(const float first, const float second, const float errorTolerance = FLOAT_SMALL_NUMBER) {
    return first > second || IsNearlyEqual(first, second, errorTolerance);
}

[[nodiscard]] inline bool IsLesserOrEqual(const float first, const float second, const float errorTolerance = FLOAT_SMALL_NUMBER) {
    return first < second || IsNearlyEqual(first, second, errorTolerance);
}

}