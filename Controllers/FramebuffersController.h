#pragma once

#include "Platform/Framebuffer.h"

#include <cstdint>
#include <memory>
#include <unordered_map>

class FramebuffersController {
public:
    void AddFramebuffer(uint32_t framebufferID, std::unique_ptr<Framebuffer> framebuffer);

    [[nodiscard]] const Framebuffer* GetFramebuffer(uint32_t framebufferID) const;

    void ResizeAll(int32_t width, int32_t height);

private:
    std::unordered_map<uint32_t, std::unique_ptr<Framebuffer>> _framebufferIDToFramebuffer{};
};
