#pragma once

#include "glad/gl.h"

#include <assimp/scene.h>

#include <string>
#include <iostream>

class Texture {
public:
	void CreateTexture(const std::string& path);

    void CreateEmbeddedTexture(const aiTexture* texture);

	void BindTexture(unsigned texNum) const;

private:
    void _createTexture(int textureWidth, int textureHeight, int nrChannels, uint8_t* textureData);

    unsigned _id = 0;
};