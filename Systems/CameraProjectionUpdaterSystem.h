#pragma once

#include "Core/Window.h"
#include "Events/ScenePanelEvents.h"

#include <entt/entt.hpp>

class CameraProjectionUpdaterSystem {
public:
    CameraProjectionUpdaterSystem(entt::registry& registry, entt::dispatcher& sceneDispatcher);

private:
    void _onFramebufferSizeChanged(WindowsEvents::FrameBufferSizeChangedEvent frameBufferSizeChangedEvent) const;

    void _onScenePanelResized(ScenePanelEvents::ScenePanelResizedEvent scenePanelResizedEvent) const;

    void _updateProjection(float width, float height) const;

    entt::registry& _registry;
};