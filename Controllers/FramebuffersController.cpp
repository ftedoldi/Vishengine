#include "FramebuffersController.h"

#include <cassert>
#include <ranges>

void FramebuffersController::AddFramebuffer(const FramebufferID framebufferID, std::unique_ptr<Framebuffer> framebuffer) {
    _framebufferIDToFramebuffer.emplace(framebufferID, std::move(framebuffer));
}

const Framebuffer* FramebuffersController::GetFramebuffer(const FramebufferID framebufferID) const {
    assert(_framebufferIDToFramebuffer.contains(framebufferID));
    return _framebufferIDToFramebuffer.find(framebufferID)->second.get();
}

void FramebuffersController::ResizeAll(const int32_t width, const int32_t height) {
    for (const auto &fb: _framebufferIDToFramebuffer | std::views::values) {
        fb->Resize(width, height);
    }
}