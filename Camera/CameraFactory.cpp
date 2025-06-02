#include "CameraFactory.h"

#include "Components/Camera/EditorCameraTag.h"
#include "Components/Camera/GameCameraTag.h"
#include "Components/Position.h"
#include "Components/Rotation.h"
#include "Components/Scale.h"
#include "Core/Window.h"

entt::entity CameraFactory::CreateEditorCamera(entt::registry& registry,
                                               const glm::vec3& worldPosition,
                                               const double fov,
                                               const double aspectRatio,
                                               const double nearPlaneZDistance,
                                               const double farPlaneZDistance,
                                               const CameraType cameraType) {
    const auto editorCamera{_createCamera(registry, worldPosition, fov, aspectRatio, nearPlaneZDistance, farPlaneZDistance, cameraType)};
    registry.emplace<EditorCameraTag>(editorCamera);
    return editorCamera;
}

entt::entity CameraFactory::CreateGameCamera(entt::registry& registry,
                                               const glm::vec3& worldPosition,
                                               const double fov,
                                               const double aspectRatio,
                                               const double nearPlaneZDistance,
                                               const double farPlaneZDistance,
                                               const CameraType cameraType) {
    const auto gameCamera{_createCamera(registry, worldPosition, fov, aspectRatio, nearPlaneZDistance, farPlaneZDistance, cameraType)};
    registry.emplace<GameCameraTag>(gameCamera);
    return gameCamera;
}

entt::entity CameraFactory::_createCamera(entt::registry& registry,
                                          const glm::vec3& worldPosition,
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

    registry.emplace<Position>(cameraEntity, worldPosition);
    registry.emplace<Scale>(cameraEntity);

    auto& rotation{registry.emplace<Rotation>(cameraEntity)};
    rotation.Quaternion = glm::quatLookAtRH(camera.Front, camera.Up);

    if(cameraType == CameraType::Perspective) {
        camera.ProjectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearPlaneZDistance, farPlaneZDistance);
    }

    return cameraEntity;
}