#include "Frustum.h"

namespace FrustumUtils {

    bool IntersectsFrustum(const Box& box, const Frustum& frustum) {
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

    Intersection ClassifyAABB(const Box& box, const Frustum& frustum) {
        bool partial = false;
        for (const auto& plane : frustum.Planes) {
            const glm::vec3& n = plane.Normal;

            // p-vertex: most aligned with plane normal (most inside)
            // n-vertex: least aligned with plane normal (most outside)
            glm::vec3 pVertex = box.Min;
            glm::vec3 nVertex = box.Max;
            if (n.x >= 0.f) { pVertex.x = box.Max.x; nVertex.x = box.Min.x; }
            if (n.y >= 0.f) { pVertex.y = box.Max.y; nVertex.y = box.Min.y; }
            if (n.z >= 0.f) { pVertex.z = box.Max.z; nVertex.z = box.Min.z; }

            if (glm::dot(n, pVertex) + plane.Distance < 0.f) return Intersection::Outside;
            if (glm::dot(n, nVertex) + plane.Distance < 0.f) partial = true;
        }
        return partial ? Intersection::Partial : Intersection::Inside;
    }

}