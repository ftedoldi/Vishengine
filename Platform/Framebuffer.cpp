#include "Framebuffer.h"

#include <glad/gl.h>

#include <cassert>

Framebuffer::Framebuffer(const int32_t positionX, const int32_t positionY, const int32_t width, const int32_t height):
    _positionX(positionX),_positionY(positionY), _width(width), _height(height) {
    _createFramebuffer();
}

Framebuffer::~Framebuffer() {
    glDeleteTextures(1, &_colorAttachment);
    glDeleteRenderbuffers(1, &_depthAttachment);
    glDeleteFramebuffers(1, &_framebuffer);
}

void Framebuffer::Bind() const {
    glViewport(_positionX, _positionY, _width, _height);
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

uint32_t Framebuffer::GetColorAttachmentID() const {
    return _colorAttachment;
}

void Framebuffer::Resize(const int32_t width, const int32_t height) {
    _width = width;
    _height = height;

    if (width == 0 || height == 0) return;

    // Delete the old framebuffer and attachments.
    if (_framebuffer != 0) {
        glDeleteFramebuffers(1, &_framebuffer);
        glDeleteTextures(1, &_colorAttachment);
        glDeleteRenderbuffers(1, &_depthAttachment);
    }

    // Recreate the framebuffer.
    _createFramebuffer();
}

void Framebuffer::_createFramebuffer() {
    glCreateFramebuffers(1, &_framebuffer);

    glCreateTextures(GL_TEXTURE_2D, 1, &_colorAttachment);
    glTextureStorage2D(_colorAttachment, 1, GL_RGB8, _width, _height);
    glTextureParameteri(_colorAttachment, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(_colorAttachment, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glNamedFramebufferTexture(_framebuffer, GL_COLOR_ATTACHMENT0, _colorAttachment, 0);

    glCreateRenderbuffers(1, &_depthAttachment);
    glNamedRenderbufferStorage(_depthAttachment, GL_DEPTH24_STENCIL8, _width, _height);
    glNamedFramebufferRenderbuffer(_framebuffer, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _depthAttachment);

    assert(glCheckNamedFramebufferStatus(_framebuffer, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}