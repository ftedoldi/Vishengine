#include "Frustum.h"

namespace FrustumUtils {

    bool IsAABBInsideFrustum(const Box& box, const Frustum& frustum) {
        for (uint32_t i{0}; i < 6; i++) {
            glm::vec3 normal{frustum.Planes[i].Normal};

            // Find the positive vertex (corner most aligned with normal).
            auto p{box.Min};
            if (Math::IsGreaterOrEqual(normal.x, 0.f)) {
                p.x = box.Max.x;
            }

            if (Math::IsGreaterOrEqual(normal.y, 0.f)) {
                p.y = box.Max.y;
            }

            if (Math::IsGreaterOrEqual(normal.z, 0.f)) {
                p.z = box.Max.z;
            }

            // If even the most inside corner is outside the whole box, cull it.
            if (const auto distance{dot(normal, p) + frustum.Planes[i].Distance}; distance < 0.) {
                return false;
            }
        }

        // If it reached here, it means it has survived all 6 planes, so it is kept
        return true;
    }
    
}