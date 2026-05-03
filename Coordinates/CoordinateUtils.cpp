#include "CoordinateUtils.h"

#include "Components/BoundingBox.h"
#include "Components/Transforms/WorldTransform.h"

namespace CoordUtils {

    Box ComputeWorldSpaceBox(const entt::entity entity, entt::registry& registry) {
        const auto& localSpaceBox{registry.get<BoundingBox>(entity).Box};
        const auto& worldTransform{registry.get<WorldTransform>(entity).Value};

        Box worldSpaceBox{
            glm::vec3{std::numeric_limits<float>::max()},
            glm::vec3{std::numeric_limits<float>::lowest()}
        };

        for (const auto vertex : localSpaceBox.GetVertices()) {
            const auto worldSpaceVertex{worldTransform.TransformPosition(vertex)};
            worldSpaceBox.Min = glm::min(worldSpaceBox.Min, worldSpaceVertex);
            worldSpaceBox.Max = glm::max(worldSpaceBox.Max, worldSpaceVertex);
        }

        return worldSpaceBox;
    }

    glm::vec3 RotateVectorByQuaternion(const glm::quat quaternion, const glm::vec3 vector) {
        const glm::quat quatPoint{vector.x, vector.y, vector.z, 0.f};

        const glm::quat partialQuatMultiply{quaternion * quatPoint};
        const glm::quat result{partialQuatMultiply * glm::inverse(quaternion)};

        return {result.x, result.y, result.z};
    }
}