#pragma once

#include "Box.h"
#include "Plane.h"

#include "glm/glm.hpp"

#include <array>

struct Frustum {
    std::array<Plane, 6> Planes{};
};

namespace FrustumUtils {

    enum class Intersection { Outside, Partial, Inside };

    bool IntersectsFrustum(const Box& box, const Frustum& frustum);

    Intersection ClassifyAABB(const Box& box, const Frustum& frustum);

}