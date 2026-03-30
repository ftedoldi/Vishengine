#pragma once

#include "Core/Window.h"

#include <entt/entt.hpp>

class CameraProjectionUpdaterSystem {
public:
    CameraProjectionUpdaterSystem(entt::registry& registry, entt::dispatcher& sceneDispatcher);

private:
    void _onFramebufferSizeChanged(WindowsEvents::FrameBufferSizeChangedEvent frameBufferSizeChangedEvent) const;

    void _onMouseMoved(WindowsEvents::MouseMovedEvent mouseMovedEvent) const;

    entt::registry& _registry;
};