#pragma once

#include <cstdint>

class Framebuffer {
public:
    Framebuffer(int32_t positionX, int32_t positionY, int32_t width, int32_t height);

    ~Framebuffer();

    void BindFrameBuffer() const;

    int32_t GetColorAttachmentID() const;

    void Resize(int32_t width, int32_t height);

private:
    void _createFramebuffer();

    uint32_t _framebuffer{};

    uint32_t _colorAttachment{};

    uint32_t _depthAttachment{};

    int32_t _positionX{};

    int32_t _positionY{};

    int32_t _width{};

    int32_t _height{};
};
