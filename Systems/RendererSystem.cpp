#include "RendererSystem.h"

RendererSystem::RendererSystem(const std::shared_ptr<Framebuffer>& sceneFramebuffer, entt::dispatcher& windowDispatcher) : _sceneFramebuffer(sceneFramebuffer) {
    windowDispatcher.sink<FrameBufferSizeChangedEvent>().connect<&RendererSystem::_onFramebufferSizeChanged>(this);
}

void RendererSystem::AddPass(std::unique_ptr<IRenderPass> pass) {
    _passes.push_back(std::move(pass));
}

void RendererSystem::Update() const {
    _sceneFramebuffer->Bind();
    for (const auto& pass : _passes) {
        pass->Execute();
    }
}

void RendererSystem::_onFramebufferSizeChanged(const FrameBufferSizeChangedEvent frameBufferSizeChangedEvent) const {
    _sceneFramebuffer->Resize(static_cast<int32_t>(frameBufferSizeChangedEvent.Width), static_cast<int32_t>(frameBufferSizeChangedEvent.Height));
}
