#include "CameraSystem.h"

#include "../Components/Transforms/WorldTransform.h"
#include "Components/Camera/Camera.h"

void CameraSystem::Update(entt::registry& registry) {
    auto view{registry.view<Camera, WorldTransform>()};

    for (auto entity: view) {
        auto& camera{view.get<Camera>(entity)};
        const auto& worldTransform{view.get<WorldTransform>(entity)};

        camera.ViewTransform = worldTransform.Value.Invert();
    }
}