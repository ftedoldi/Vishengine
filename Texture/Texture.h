#pragma once

#include "glad/gl.h"

#include <assimp/scene.h>

#include <string>
#include <iostream>

namespace TextureUtils {
    struct TextureInfo {
        int width{};
        int height{};
        int nrChannels{};
        uint8_t* textureData{nullptr};
    };
}

class Texture {
public:
	void CreateTexture(const std::string& path);

    void CreateEmbeddedTexture(const aiTexture* texture);

	void BindTexture(unsigned texNum) const;

private:
    void _createTexture(const TextureUtils::TextureInfo& textureInfo);

    unsigned _id = 0;
};