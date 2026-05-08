#pragma once

#include <assimp/scene.h>

#include <string>
#include <iostream>

class Texture {
public:
    ~Texture();

	void CreateTexture(const std::string& path);

    void CreateEmbeddedTexture(const aiTexture* texture);

	void BindTexture(unsigned texNum) const;

private:
    void _createTexture(int textureWidth, int textureHeight, int nrChannels, uint8_t* textureData);

    unsigned _id{0};
};