#pragma once

#include "glad/gl.h"
#include <assimp/scene.h>

#include <iostream>
#include <string>
#include <utility>

class Texture {
public:
    Texture() = default;
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    Texture(Texture&& other) noexcept : _id{std::exchange(other._id, 0)} {
    }

    Texture& operator=(Texture&& other) noexcept {
        if (this != &other) {
            if (_id) {
                glDeleteTextures(1, &_id);
            }
            _id = std::exchange(other._id, 0);
        }
        return *this;
    }

	void Create(const std::string& path);

    void CreateEmbeddedTexture(const aiTexture* texture);

	void BindTexture(unsigned texNum) const;

private:
    void _createTexture(int textureWidth, int textureHeight, int nrChannels, uint8_t* textureData);

    unsigned _id{0};
};