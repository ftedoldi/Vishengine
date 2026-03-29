#pragma once

#include "Core/Window.h"
#include "IRenderPass.h"
#include "Platform/Framebuffer.h"

#include <memory>
#include <vector>

class RendererSystem {
public:
    RendererSystem(std::shared_ptr<Framebuffer> sceneFramebuffer, entt::dispatcher& windowDispatcher);

    void AddPass(std::unique_ptr<IRenderPass> pass);

    void Update() const;

private:
    void _onFramebufferSizeChanged(FrameBufferSizeChangedEvent frameBufferSizeChangedEvent) const;

    std::vector<std::unique_ptr<IRenderPass>> _passes{};

    std::shared_ptr<Framebuffer> _sceneFramebuffer{};
};
