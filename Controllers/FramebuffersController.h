#pragma once

#include "Components/RenderingComponents.h"
#include "Platform/Framebuffer.h"

#include <ankerl/unordered_dense.h>

#include <cstdint>
#include <memory>

class FramebuffersController {
public:
    void AddFramebuffer(FramebufferID framebufferID, std::unique_ptr<Framebuffer> framebuffer);

    [[nodiscard]] const Framebuffer* GetFramebuffer(FramebufferID framebufferID) const;

    void ResizeAll(int32_t width, int32_t height);

private:
    ankerl::unordered_dense::map<FramebufferID, std::unique_ptr<Framebuffer>> _framebufferIDToFramebuffer{};
};
