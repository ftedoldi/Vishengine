#pragma once

#include "Components/RenderingComponents.h"
#include "Platform/Framebuffer.h"

#include <cstdint>
#include <memory>
#include <unordered_map>

class FramebuffersController {
public:
    void AddFramebuffer(FramebufferID framebufferID, std::unique_ptr<Framebuffer> framebuffer);

    [[nodiscard]] const Framebuffer* GetFramebuffer(FramebufferID framebufferID) const;

    void ResizeAll(int32_t width, int32_t height);

private:
    std::unordered_map<FramebufferID, std::unique_ptr<Framebuffer>> _framebufferIDToFramebuffer{};
};
