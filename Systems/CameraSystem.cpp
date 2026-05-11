#include "CameraSystem.h"

#include "Components/Camera/Camera.h"
#include "Components/Transforms/WorldTransform.h"
#include "glm/gtc/matrix_access.hpp"

namespace {

    Plane ExtractPlane(const glm::vec4& row) {
        const float length{glm::length(glm::vec3{row.x, row.y, row.z})};
        return { glm::vec3{row.x, row.y, row.z} / length, row.w / length };
    }

}

void CameraSystem::Update(entt::registry& registry) {
    // TODO: this should be computed only if the camera has changed the world transform.
    const auto view{registry.view<Camera, WorldTransform>()};

    for (const auto entity: view) {
        auto& camera{view.get<Camera>(entity)};
        const auto& worldTransform{view.get<WorldTransform>(entity).Value};

        camera.ViewTransform = worldTransform.Invert();

        // compute the frustum
        const auto clipMatrix{camera.ProjectionMatrix * camera.ViewTransform.ToMatrix()};
        const auto r0{glm::row(clipMatrix, 0)};
        const auto r1{glm::row(clipMatrix, 1)};
        const auto r2{glm::row(clipMatrix, 2)};
        const auto r3{glm::row(clipMatrix, 3)};
        camera.ViewFrustum = {std::array{
            ExtractPlane(r0 + r3),  // left
            ExtractPlane(r3 - r0),  // right
            ExtractPlane(r1 + r3),  // bottom
            ExtractPlane(r3 - r1),  // top
            ExtractPlane(r2 + r3),  // near
            ExtractPlane(r3 - r2),  // far
        }};
    }
}