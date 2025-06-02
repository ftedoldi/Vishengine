#pragma once

#include "Components/Camera/Camera.h"
#include "Components/Rotation.h"

#include "Libs/entt/src/entt/entt.hpp"

enum class CameraType {
    Perspective,
    Orthogonal,
};

class CameraFactory {
public:
    static entt::entity CreateEditorCamera(entt::registry& registry,
                                    const glm::vec3& position,
                                    double fov,
                                    double aspectRatio,
                                    double nearPlaneZDistance,
                                    double farPlaneZDistance,
                                    CameraType cameraType);

    static entt::entity CreateGameCamera(entt::registry& registry,
                                         const glm::vec3& worldPosition,
                                         double fov,
                                         double aspectRatio,
                                         double nearPlaneZDistance,
                                         double farPlaneZDistance,
                                         CameraType cameraType);

private:
    static entt::entity _createCamera(entt::registry& registry,
                                      const glm::vec3& worldPosition,
                                      double fov,
                                      double aspectRatio,
                                      double nearPlaneZDistance,
                                      double farPlaneZDistance,
                                      CameraType cameraType);
};