#include "CreateCameraSystem.h"

#include "Components/CameraComponents/Perspective.h"
#include "Components/Window.h"

CreateCameraSystem::CreateCameraSystem(entt::registry& registry, entt::entity window) : _registry{registry}, _window{window} {

}

entt::entity CreateCameraSystem::CreateCamera(CameraType cameraType) const {
    auto cameraEntity{_registry.create()};

    auto& camera{_registry.emplace<Camera>(cameraEntity)};
    auto& transform{_registry.emplace<Transform>(cameraEntity)};

    transform.Translation = camera.Position;

    if(cameraType == CameraType::Perspective) {
        transform.Rotation = glm::quatLookAtRH(camera.Front, camera.Up);
        _setupPerspective(cameraEntity);
    }
    else if(cameraType == CameraType::Orthogonal) {
        // TODO: implement this whenever orthogonal camera is needed.
    }
    return cameraEntity;
}

void CreateCameraSystem::_setupPerspective(const entt::entity cameraEntity) const {
    auto& perspective{_registry.emplace<Perspective>(cameraEntity)};
    
    auto windowView{_registry.view<Window>()};
    auto& window{windowView.get<Window>(_window)};

    // TODO: maybe I can pass even the fov or the near and far plane idk.
    perspective.Matrix = glm::perspective(glm::radians(45.f), static_cast<float>(window.Width) / static_cast<float>(window.Height), 0.1f, 100.f);
}

