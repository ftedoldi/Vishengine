#include "Framebuffer.h"

#include <glad/gl.h>

#include <cassert>

Framebuffer::Framebuffer(const int32_t positionX, const int32_t positionY, const int32_t width, const int32_t height):
    _positionX(positionX),_positionY(positionY), _width(width), _height(height) {
    _createFramebuffer();
}

Framebuffer::~Framebuffer() {
    glDeleteBuffers(1, &_colorAttachment);
    glDeleteRenderbuffers(1, &_depthAttachment);
    glDeleteFramebuffers(1, &_framebuffer);
}

void Framebuffer::BindFrameBuffer() const {
    glViewport(_positionX, _positionY, _width, _height);
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

int32_t Framebuffer::GetColorAttachmentID() const {
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
    glGenFramebuffers(1, &_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);

    glGenTextures(1, &_colorAttachment);
    glBindTexture(GL_TEXTURE_2D, _colorAttachment);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _colorAttachment, 0);

    glGenRenderbuffers(1, &_depthAttachment);
    glBindRenderbuffer(GL_RENDERBUFFER, _depthAttachment);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _width, _height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _depthAttachment);
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}