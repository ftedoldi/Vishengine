#include "CameraProjectionUpdaterSystem.h"

#include "Components/Camera/Camera.h"
#include "Events/WindowEvents.h"

#include "Math/Math.h"
#include "glm/glm.hpp"

CameraProjectionUpdaterSystem::CameraProjectionUpdaterSystem(entt::registry& registry, entt::dispatcher& sceneDispatcher) : _registry{registry} {
    sceneDispatcher.sink<WindowsEvents::FrameBufferSizeChangedEvent>().connect<&CameraProjectionUpdaterSystem::_onFramebufferSizeChanged>(this);
    sceneDispatcher.sink<ScenePanelEvents::ScenePanelResizedEvent>().connect<&CameraProjectionUpdaterSystem::_onScenePanelResized>(this);
}

void CameraProjectionUpdaterSystem::_onFramebufferSizeChanged(const WindowsEvents::FrameBufferSizeChangedEvent frameBufferSizeChangedEvent) const {
    _updateProjection(frameBufferSizeChangedEvent.Width, frameBufferSizeChangedEvent.Height);
}

void CameraProjectionUpdaterSystem::_onScenePanelResized(const ScenePanelEvents::ScenePanelResizedEvent scenePanelResizedEvent) const {
    _updateProjection(scenePanelResizedEvent.ScenePanelSize.x, scenePanelResizedEvent.ScenePanelSize.y);
}

void CameraProjectionUpdaterSystem::_updateProjection(const float width, const float height) const {
    if (height < 0.f || Math::IsNearlyEqual(height, 0.f)) {
        return;
    }
    const auto view{_registry.view<Camera>()};
    for (const auto entity : view) {
        auto& camera{view.get<Camera>(entity)};
        camera.ProjectionMatrix = glm::perspective(glm::radians(camera.FOV), width / height, camera.NearPlaneZDistance, camera.FarPlaneZDistance);
    }
}