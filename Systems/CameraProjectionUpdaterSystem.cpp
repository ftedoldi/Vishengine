#include "CameraProjectionUpdaterSystem.h"
#include "Platform/Mouse.h"

#include "Components/CameraComponents/ActiveCameraTag.h"
#include "Components/CameraComponents/Camera.h"
#include "Components/Rotation.h"
#include "glm/glm.hpp"
#include "glm/ext/quaternion_trigonometric.hpp"

CameraProjectionUpdaterSystem::CameraProjectionUpdaterSystem(entt::registry& registry, entt::dispatcher& windowDispatcher) : _registry{registry} {
    windowDispatcher.sink<FrameBufferSizeChangedEvent>().connect<&CameraProjectionUpdaterSystem::OnFramebufferSizeChanged>(this);
    windowDispatcher.sink<MouseMovedEvent>().connect<&CameraProjectionUpdaterSystem::OnMouseMoved>(this);
}

void CameraProjectionUpdaterSystem::OnFramebufferSizeChanged(FrameBufferSizeChangedEvent frameBufferSizeChangedEvent) {
    auto view{_registry.view<Camera, ActiveCameraTag>()};
    for(const auto entity: view) {
        auto& cameraComponent{view.get<Camera>(entity)};
        const auto newHeight{frameBufferSizeChangedEvent.Height};
        const auto newWidth{frameBufferSizeChangedEvent.Width};
        cameraComponent.ProjectionMatrix = glm::perspective(glm::radians(cameraComponent.FOV),
                                                            newHeight / newWidth,
                                                            cameraComponent.NearPlaneZDistance,
                                                            cameraComponent.FarPlaneZDistance);
    }
}

void CameraProjectionUpdaterSystem::OnMouseMoved(const MouseMovedEvent mouseMovedEvent) {
    auto view{_registry.view<Camera, ActiveCameraTag, Rotation>()};

    for(const auto entity: view) {
        auto& cameraComponent{view.get<Camera>(entity)};
        auto& rotationComponent{view.get<Rotation>(entity)};
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

        const float sensitivity{0.1f};
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

        rotationComponent.Quaternion = glm::quatLookAtRH(cameraComponent.Front, cameraComponent.Up);
    }

}
