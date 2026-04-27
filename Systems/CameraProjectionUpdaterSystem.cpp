#include "CameraProjectionUpdaterSystem.h"

#include "Components/Camera/Camera.h"
#include "Events/WindowEvents.h"

#include "glm/glm.hpp"

CameraProjectionUpdaterSystem::CameraProjectionUpdaterSystem(entt::registry& registry, entt::dispatcher& sceneDispatcher) : _registry{registry} {
    sceneDispatcher.sink<WindowsEvents::FrameBufferSizeChangedEvent>().connect<&CameraProjectionUpdaterSystem::_onFramebufferSizeChanged>(this);
}

void CameraProjectionUpdaterSystem::_onFramebufferSizeChanged(const WindowsEvents::FrameBufferSizeChangedEvent frameBufferSizeChangedEvent) const {
    const auto view{_registry.view<Camera>()};
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