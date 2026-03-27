#include "CameraProjectionUpdaterSystem.h"

#include "Components/Camera/ActiveCameraTag.h"
#include "Components/Camera/Camera.h"
#include "Components/Camera/EditorCameraTag.h"
#include "Components/Transforms/RelativeTransform.h"
#include "Components/Transforms/TransformDirtyFlag.h"
#include "Platform/Mouse.h"

#include "glm/ext/quaternion_trigonometric.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

CameraProjectionUpdaterSystem::CameraProjectionUpdaterSystem(entt::registry& registry, entt::dispatcher& windowDispatcher) : _registry{registry} {
    windowDispatcher.sink<FrameBufferSizeChangedEvent>().connect<&CameraProjectionUpdaterSystem::_onFramebufferSizeChanged>(this);
    windowDispatcher.sink<MouseMovedEvent>().connect<&CameraProjectionUpdaterSystem::_onMouseMoved>(this);
}

void CameraProjectionUpdaterSystem::_onFramebufferSizeChanged(const FrameBufferSizeChangedEvent frameBufferSizeChangedEvent) const {
    auto view{_registry.view<Camera>()};
    for(const auto entity: view) {
        auto& cameraComponent{view.get<Camera>(entity)};
        const auto newHeight{frameBufferSizeChangedEvent.Height};
        const auto newWidth{frameBufferSizeChangedEvent.Width};
        if(newHeight > 0) {
            cameraComponent.ProjectionMatrix = glm::perspective(glm::radians(cameraComponent.FOV),
                                                                newWidth / newHeight,
                                                                cameraComponent.NearPlaneZDistance,
                                                                cameraComponent.FarPlaneZDistance);
        }
    }
}

void CameraProjectionUpdaterSystem::_onMouseMoved(const MouseMovedEvent mouseMovedEvent) const {
    auto view{_registry.view<Camera, ActiveCameraTag, EditorCameraTag, RelativeTransform, TransformDirtyFlag>()};

    for(const auto entity: view) {
        auto& cameraComponent{view.get<Camera>(entity)};
        auto& relativeTransform{view.get<RelativeTransform>(entity)};
        auto& transformFlag{view.get<TransformDirtyFlag>(entity)};
        transformFlag.ShouldUpdateTransform = true;
        // Setup editor camera mouse rotation.
        if (Mouse::FirstTimeMovingMouse){
            Mouse::LastXPos = mouseMovedEvent.XMousePosition;
            Mouse::LastYPos = mouseMovedEvent.YMousePosition;
            Mouse::FirstTimeMovingMouse = false;
        }

        double xOffset{mouseMovedEvent.XMousePosition - Mouse::LastXPos};
        double yOffset{Mouse::LastYPos - mouseMovedEvent.YMousePosition};

        Mouse::LastXPos = mouseMovedEvent.XMousePosition;
        Mouse::LastYPos = mouseMovedEvent.YMousePosition;

        constexpr float sensitivity{0.1f};
        xOffset *= sensitivity;
        yOffset *= sensitivity;

        cameraComponent.YawAngle += static_cast<float>(xOffset);
        cameraComponent.PitchAngle += static_cast<float>(yOffset);

        if(cameraComponent.PitchAngle > 89.f)
            cameraComponent.PitchAngle = 89.f;
        if(cameraComponent.PitchAngle < -89.f)
            cameraComponent.PitchAngle = -89.f;

        const auto qx{glm::angleAxis(glm::radians(cameraComponent.PitchAngle), glm::vec3{1., 0., 0.})};
        const auto qy{glm::angleAxis(glm::radians(-cameraComponent.YawAngle), glm::vec3{0., 1., 0.})};
        const auto q{qy * qx};

        cameraComponent.Front = glm::normalize(q * glm::vec3{0.0, 0.0, -1.0});
        cameraComponent.Up = glm::normalize(q * glm::vec3{0.0, 1.0, 0.0});

        relativeTransform.Value.Rotation = glm::quatLookAtRH(cameraComponent.Front, cameraComponent.Up);
    }
}
