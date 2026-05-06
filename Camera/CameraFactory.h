#pragma once

#include "Components/Camera/Camera.h"

#include <entt/entt.hpp>

enum class CameraType {
    Perspective,
    Orthogonal,
};

class CameraFactory {
public:
    static entt::entity CreateEditorCamera(entt::registry& registry,
                                   glm::vec3 worldPosition,
                                   float fov,
                                   float aspectRatio,
                                   float nearPlaneZDistance,
                                   float farPlaneZDistance,
                                   CameraType cameraType);

    static entt::entity CreateGameCamera(entt::registry& registry,
                                         glm::vec3 worldPosition,
                                         float fov,
                                         float aspectRatio,
                                         float nearPlaneZDistance,
                                         float farPlaneZDistance,
                                         CameraType cameraType);

private:
    static entt::entity _createCamera(entt::registry& registry,
                                      glm::vec3 worldPosition,
                                      float fov,
                                      float aspectRatio,
                                      float nearPlaneZDistance,
                                      float farPlaneZDistance,
                                      CameraType cameraType);
};