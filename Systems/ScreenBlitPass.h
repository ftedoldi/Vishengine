#pragma once

#include "IRenderPass.h"

#include "Platform/Framebuffer.h"
#include "Shaders/Shader.h"

class ScreenBlitPass : public IRenderPass {
public:
    ScreenBlitPass(std::unique_ptr<Shader> screenShader, const std::shared_ptr<Framebuffer>& sourceFramebuffer);

    ~ScreenBlitPass() override;

    void Execute() override;

private:
    void _initQuad();

    std::unique_ptr<Shader> _screenShader{};
    std::shared_ptr<Framebuffer> _sourceFramebuffer{};

    uint32_t _quadVao{0};
    uint32_t _quadVbo{0};
};
