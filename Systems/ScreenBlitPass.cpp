#include "ScreenBlitPass.h"

#include <glad/gl.h>

ScreenBlitPass::ScreenBlitPass(std::unique_ptr<Shader> screenShader, const std::shared_ptr<Framebuffer>& sourceFramebuffer)
    : _screenShader{std::move(screenShader)}, _sourceFramebuffer{sourceFramebuffer} {
    _initQuad();

    _screenShader->UseProgram();
    _screenShader->SetUniformInt("screenTexture", 0);
}

ScreenBlitPass::~ScreenBlitPass() {
    glDeleteVertexArrays(1, &_quadVao);
    glDeleteBuffers(1, &_quadVbo);
    _screenShader->DeleteProgram();
}

void ScreenBlitPass::Execute() {
    // Switch to the default framebuffer and clear it.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    _screenShader->UseProgram();

    // Bind the scene color attachment to texture unit 0.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(_sourceFramebuffer->GetColorAttachmentID()));

    glBindVertexArray(_quadVao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void ScreenBlitPass::_initQuad() {
    constexpr float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &_quadVao);
    glGenBuffers(1, &_quadVbo);

    glBindVertexArray(_quadVao);
    glBindBuffer(GL_ARRAY_BUFFER, _quadVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          reinterpret_cast<void*>(2 * sizeof(float)));

    glBindVertexArray(0);
}
