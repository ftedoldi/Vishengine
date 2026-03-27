#include "CameraFactory.h"

#include "Components/Camera/EditorCameraTag.h"
#include "Components/Camera/GameCameraTag.h"
#include "Components/Relationship.h"
#include "Components/Transforms/RelativeTransform.h"
#include "Components/Transforms/TransformDirtyFlag.h"
#include "Components/Transforms/WorldTransform.h"

#include "glm/gtc/quaternion.hpp"

entt::entity CameraFactory::CreateEditorCamera(entt::registry& registry,
                                               const glm::vec3& worldPosition,
                                               const float fov,
                                               const float aspectRatio,
                                               const float nearPlaneZDistance,
                                               const float farPlaneZDistance,
                                               const CameraType cameraType) {
    const auto editorCamera{_createCamera(registry, worldPosition, fov, aspectRatio, nearPlaneZDistance, farPlaneZDistance, cameraType)};
    registry.emplace<EditorCameraTag>(editorCamera);
    return editorCamera;
}

entt::entity CameraFactory::CreateGameCamera(entt::registry& registry,
                                               const glm::vec3& worldPosition,
                                               const float fov,
                                               const float aspectRatio,
                                               const float nearPlaneZDistance,
                                               const float farPlaneZDistance,
                                               const CameraType cameraType) {
    const auto gameCamera{_createCamera(registry, worldPosition, fov, aspectRatio, nearPlaneZDistance, farPlaneZDistance, cameraType)};
    registry.emplace<GameCameraTag>(gameCamera);
    return gameCamera;
}

entt::entity CameraFactory::_createCamera(entt::registry& registry,
                                          const glm::vec3& worldPosition,
                                          const float fov,
                                          const float aspectRatio,
                                          const float nearPlaneZDistance,
                                          const float farPlaneZDistance,
                                          const CameraType cameraType) {
    auto cameraEntity{registry.create()};

    registry.emplace<TransformDirtyFlag>(cameraEntity);
    auto& camera{registry.emplace<Camera>(cameraEntity)};
    camera.FOV = fov;
    camera.AspectRatio = aspectRatio;
    camera.NearPlaneZDistance = nearPlaneZDistance;
    camera.FarPlaneZDistance = farPlaneZDistance;

    registry.emplace<Relationship>(cameraEntity);
    auto& relativeTransform{registry.emplace<RelativeTransform>(cameraEntity).Value};
    relativeTransform.Position = worldPosition;
    relativeTransform.Rotation = glm::quatLookAtRH(camera.Front, camera.Up);

    registry.emplace<WorldTransform>(cameraEntity);

    if(cameraType == CameraType::Perspective) {
        camera.ProjectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearPlaneZDistance, farPlaneZDistance);
    }

    return cameraEntity;
}