#pragma once

#include "Core/Window.h"

#include <entt/entt.hpp>

class CameraProjectionUpdaterSystem {
public:
    CameraProjectionUpdaterSystem(entt::registry& registry, entt::dispatcher& windowDispatcher);

private:
    void _onFramebufferSizeChanged(FrameBufferSizeChangedEvent frameBufferSizeChangedEvent) const;

    void _onMouseMoved(MouseMovedEvent mouseMovedEvent) const;

    entt::registry& _registry;
};