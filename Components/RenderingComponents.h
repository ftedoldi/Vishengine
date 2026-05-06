#pragma once

#include <bitset>
#include <cstdint>

enum class RenderLayer {
    SceneMeshes,
    //DebugBoxColliders,
    //DebugOctree,
    DebugFrustumIntersections,
};

enum class FramebufferID : uint32_t {
    Main,
    //Shadow = 1,
    FrustumDebugView,
};

enum class ShaderID : uint32_t {
    Standard,
    //Skybox = 1,
    FrustumDebug,
};

struct RenderLayers {
    std::bitset<32> Layers{};
};

struct RenderTarget {
    FramebufferID FramebufferHandle{};
};

struct RenderPass {
    ShaderID ShaderHandle{};
};

struct LitPassTag {
};