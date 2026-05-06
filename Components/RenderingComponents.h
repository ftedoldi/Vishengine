#pragma once

#include <cstdint>

struct RenderTarget {
    uint32_t FramebufferHandle{};
};

struct RenderPass {
    uint32_t ShaderHandle{};
};

struct MainViewTag {
};

struct LitPassTag {
};