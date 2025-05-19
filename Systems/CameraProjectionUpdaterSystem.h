#pragma once

#include "Core/Window.h"

#include <entt/entt.hpp>

class CameraProjectionUpdaterSystem {
public:
    CameraProjectionUpdaterSystem(entt::registry& registry, entt::dispatcher& windowDispatcher);

    void OnFramebufferSizeChanged(FrameBufferSizeChangedEvent frameBufferSizeChangedEvent);

    void OnMouseMoved(MouseMovedEvent mouseMovedEvent);

private:
    entt::registry& _registry;
};