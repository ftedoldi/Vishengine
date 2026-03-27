#pragma once

#include "Components/Camera/Camera.h"

#include <entt/entt.hpp>

enum class CameraType {
    Perspective,
    Orthogonal,
};

class CameraFactory {
public:
    static auto CreateEditorCamera(entt::registry& registry,
                                   const glm::vec3& worldPosition,
                                   float fov,
                                   float aspectRatio,
                                   float nearPlaneZDistance,
                                   float farPlaneZDistance,
                                   CameraType cameraType) -> entt::entity;

    static entt::entity CreateGameCamera(entt::registry& registry,
                                         const glm::vec3& worldPosition,
                                         float fov,
                                         float aspectRatio,
                                         float nearPlaneZDistance,
                                         float farPlaneZDistance,
                                         CameraType cameraType);

private:
    static entt::entity _createCamera(entt::registry& registry,
                                      const glm::vec3& worldPosition,
                                      float fov,
                                      float aspectRatio,
                                      float nearPlaneZDistance,
                                      float farPlaneZDistance,
                                      CameraType cameraType);
};