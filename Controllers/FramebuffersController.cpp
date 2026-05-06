#include "FramebuffersController.h"

#include <ranges>

void FramebuffersController::AddFramebuffer(const uint32_t framebufferID, std::unique_ptr<Framebuffer> framebuffer) {
    _framebufferIDToFramebuffer.emplace(framebufferID, std::move(framebuffer));
}

const Framebuffer* FramebuffersController::GetFramebuffer(const uint32_t framebufferID) const {
    assert(_framebufferIDToFramebuffer.contains(framebufferID));
    return _framebufferIDToFramebuffer.find(framebufferID)->second.get();
}

void FramebuffersController::ResizeAll(const int32_t width, const int32_t height) {
    for (const auto &fb: _framebufferIDToFramebuffer | std::views::values) {
        fb->Resize(width, height);
    }
}