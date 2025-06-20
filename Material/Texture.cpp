#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void Texture::CreateTexture(const std::string& path) {
	int textureWidth{}, textureHeight{}, nrChannels{};

	stbi_set_flip_vertically_on_load(true);
	auto* const textureData{stbi_load(path.c_str(), &textureWidth, &textureHeight, &nrChannels, 0)};
	stbi_set_flip_vertically_on_load(false);

    _createTexture(textureWidth, textureHeight, nrChannels, textureData);
}

void Texture::BindTexture(const unsigned texNum) const {
	glBindTextureUnit(texNum, _id);
}

void Texture::CreateEmbeddedTexture(const aiTexture* const texture) {
    int textureWidth{}, textureHeight{}, nrChannels{};

    uint8_t* textureData{};
    if(texture->mHeight == 0) {
        textureData = stbi_load_from_memory(reinterpret_cast<uint8_t*>(texture->pcData), texture->mWidth, &textureWidth, &textureHeight, &nrChannels, 0);
    } else {
        textureData = stbi_load_from_memory(reinterpret_cast<uint8_t*>(texture->pcData), texture->mWidth * texture->mHeight, &textureWidth, &textureHeight, &nrChannels, 0);
    }

    _createTexture(textureWidth, textureHeight, nrChannels, textureData);
}

void Texture::_createTexture(const int textureWidth, const int textureHeight, const int nrChannels, uint8_t* const textureData) {
    glCreateTextures(GL_TEXTURE_2D, static_cast<GLsizei>(1), &_id);

    glTextureParameteri(_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(_id, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTextureParameteri(_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // TODO: remove magic numbers with actual parameters

    if (textureData) {
        int sizedInternalFormat{};
        int internalFormat{};

        switch(nrChannels) {
            case 1:
                internalFormat = GL_RED;
                sizedInternalFormat = GL_R8;
                break;
            case 2:
                internalFormat = GL_RG;
                sizedInternalFormat = GL_RG8;
                break;
            case 3:
                internalFormat = GL_RGB;
                sizedInternalFormat = GL_RGB8;
                break;
            case 4:
                internalFormat = GL_RGBA;
                sizedInternalFormat = GL_RGBA8;
                break;
            default: break;
        }

        // TODO: check how to set various mip-map levels
        glTextureStorage2D(_id, 1, sizedInternalFormat, textureWidth, textureHeight);
        glTextureSubImage2D(_id, 0, 0, 0, textureWidth, textureHeight, internalFormat, GL_UNSIGNED_BYTE, textureData);
        glGenerateTextureMipmap(_id);
    }
    else
        std::cout << "Cannot load texture" << std::endl;

    stbi_image_free(textureData);
}
