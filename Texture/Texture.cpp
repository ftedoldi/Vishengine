#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void Texture::CreateTexture(const std::string& path) {
	int width, height, nrChannels;

	stbi_set_flip_vertically_on_load(true);
	auto* const textureData{stbi_load(path.c_str(), &width, &height, &nrChannels, 0)};
	stbi_set_flip_vertically_on_load(false);

    TextureUtils::TextureInfo textureInfo;
    textureInfo.width = width;
    textureInfo.height = height;
    textureInfo.nrChannels = nrChannels;
    textureInfo.textureData = textureData;

    _createTexture(textureInfo);
}

void Texture::BindTexture(const unsigned texNum) const {
	glBindTextureUnit(texNum, _id);
}

void Texture::CreateEmbeddedTexture(const aiTexture* const texture) {
    int width, height, nrChannels;

    uint8_t* textureData{};
    if(texture->mHeight == 0) {
        textureData = stbi_load_from_memory(reinterpret_cast<uint8_t*>(texture->pcData), texture->mWidth, &width, &height, &nrChannels, 0);
    } else {
        textureData = stbi_load_from_memory(reinterpret_cast<uint8_t*>(texture->pcData), texture->mWidth * texture->mHeight, &width, &height, &nrChannels, 0);
    }

    TextureUtils::TextureInfo textureInfo;
    textureInfo.width = width;
    textureInfo.height = height;
    textureInfo.nrChannels = nrChannels;
    textureInfo.textureData = textureData;

    _createTexture(textureInfo);
}

void Texture::_createTexture(const TextureUtils::TextureInfo& textureInfo) {
    glCreateTextures(GL_TEXTURE_2D, static_cast<GLsizei>(1), &_id);

    glTextureParameteri(_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(_id, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTextureParameteri(_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // TODO: remove magic numbers with actual parameters

    if (textureInfo.textureData) {
        int sizedInternalFormat{};
        int internalFormat{};

        switch(textureInfo.nrChannels) {
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
        glTextureStorage2D(_id, 1, sizedInternalFormat, textureInfo.width, textureInfo.height);
        glTextureSubImage2D(_id, 0, 0, 0, textureInfo.width, textureInfo.height, internalFormat, GL_UNSIGNED_BYTE, textureInfo.textureData);
        glGenerateTextureMipmap(_id);
    }
    else
        std::cout << "Cannot load texture" << std::endl;

    stbi_image_free(textureInfo.textureData);
}
