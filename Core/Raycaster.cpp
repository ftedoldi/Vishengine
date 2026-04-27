#include "Raycaster.h"

#include <algorithm>
#include <cmath>

bool Raycaster::IntersectBox(const Ray& ray, const Box& box, float& distance) {
    constexpr float epsilon{1e-6f};

    float tMin{0.f};
    float tMax{ray.MaxDistance};

    for (int axis{0}; axis < 3; ++axis) {
        const float origin{ray.Origin[axis]};
        const float direction{ray.Direction[axis]};
        const float min{box.Min[axis]};
        const float max{box.Max[axis]};

        if (std::abs(direction) < epsilon) {
            // The ray is parallel to this slab. It can hit the box only if the
            // origin is already inside the slab.
            if (origin < min || origin > max) {
                return false;
            }
            continue;
        }

        const float inverseDirection{1.f / direction};
        float t1{(min - origin) * inverseDirection};
        float t2{(max - origin) * inverseDirection};

        if (t1 > t2) {
            std::swap(t1, t2);
        }

        tMin = std::max(tMin, t1);
        tMax = std::min(tMax, t2);

        if (tMin > tMax) {
            return false;
        }
    }

    distance = tMin;
    return true;
}
