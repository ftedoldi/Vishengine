#pragma once

#include "Box.h"
#include "Plane.h"

#include "glm/glm.hpp"

#include <array>

struct Frustum {
    std::array<Plane, 6> Planes{};
};

namespace FrustumUtils {

    bool IsAABBInsideFrustum(const Box& box, const Frustum& frustum);

}