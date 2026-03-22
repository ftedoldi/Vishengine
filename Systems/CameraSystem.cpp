#include "CameraSystem.h"

#include "Components/Camera/Camera.h"
#include "Components/WorldTransform.h"

Transform CalculateViewTransform(const Transform& transform, const Transform& cameraTransform) {
    const auto inverseCameraTransform{cameraTransform.Invert()};

    return inverseCameraTransform.Cumulate(transform);
}


void CameraSystem::Update(entt::registry& registry) {
    auto view{registry.view<Camera, WorldTransform>()};

    for (auto entity: view) {
        auto& camera{view.get<Camera>(entity)};
        const auto& worldTransform{view.get<WorldTransform>(entity)};

        camera.ViewTransform = worldTransform.Value.Invert();
    }
}