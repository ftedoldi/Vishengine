#include "CreateCameraSystem.h"

#include "Components/CameraComponents/EditorCameraTag.h"
#include "Components/CameraComponents/GameCameraTag.h"
#include "Components/Position.h"
#include "Components/Rotation.h"
#include "Components/Scale.h"
#include "Core/Window.h"

entt::entity CameraFactory::CreateEditorCamera(entt::registry& registry,
                                               const glm::vec3& position,
                                               const double fov,
                                               const double aspectRatio,
                                               const double nearPlaneZDistance,
                                               const double farPlaneZDistance,
                                               const CameraType cameraType) {
        const auto editorCamera{_createCamera(registry, position, fov, aspectRatio, nearPlaneZDistance, farPlaneZDistance, cameraType)};
        registry.emplace<EditorCameraTag>(editorCamera);
        return editorCamera;
}

entt::entity CameraFactory::CreateGameCamera(entt::registry& registry,
                                               const glm::vec3& position,
                                               const double fov,
                                               const double aspectRatio,
                                               const double nearPlaneZDistance,
                                               const double farPlaneZDistance,
                                               const CameraType cameraType) {
    const auto gameCamera{_createCamera(registry, position, fov, aspectRatio, nearPlaneZDistance, farPlaneZDistance, cameraType)};
    registry.emplace<GameCameraTag>(gameCamera);
    return gameCamera;
}

entt::entity CameraFactory::_createCamera(entt::registry& registry,
                                          const glm::vec3& position,
                                          const double fov,
                                          const double aspectRatio,
                                          const double nearPlaneZDistance,
                                          const double farPlaneZDistance,
                                          const CameraType cameraType) {
    auto cameraEntity{registry.create()};

    auto& camera{registry.emplace<Camera>(cameraEntity)};
    camera.FOV = fov;
    camera.AspectRatio = aspectRatio;
    camera.NearPlaneZDistance = nearPlaneZDistance;
    camera.FarPlaneZDistance = farPlaneZDistance;

    registry.emplace<Position>(cameraEntity, position);
    registry.emplace<Scale>(cameraEntity);

    auto& rotation{registry.emplace<Rotation>(cameraEntity)};
    rotation.Quaternion = glm::quatLookAtRH(camera.Front, camera.Up);

    if(cameraType == CameraType::Perspective) {
        camera.ProjectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearPlaneZDistance, farPlaneZDistance);
    }

    return cameraEntity;
}