#pragma once

#include "Components/CameraComponents/Camera.h"
#include "Components/Transform.h"

#include "Texture/Texture.h"
#include "Shaders/Shader.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <iostream>
#include <vector>
#include <memory>

class Mesh {
public:
    Mesh(std::vector<glm::vec3> vertices, std::vector<glm::vec2> textureCoords, std::vector<unsigned int> indices);
    ~Mesh();

    std::vector<unsigned int> Indices{};
    unsigned Vao{0};

    std::vector<Texture> TexturesDiffuse{};
    std::vector<Texture> TexturesSpecular{};
    std::vector<Texture> TexturesNormal{};

    void SetColorDiffuse(glm::vec4 colorDiffuse);
    glm::vec4 GetColorDiffuse() const;

    void SetColorSpecular(glm::vec3 colorSpecular);
    glm::vec3 GetColorSpecular() const;

    void SetHasTextureDiffuse(bool hasTextureDiffuse);
    bool GetHasTextureDiffuse() const;

    void SetHasTextureSpecular(bool hasTextureSpecular);
    bool GetHasTextureSpecular() const;

private:
    std::vector<glm::vec3> _vertices{};
    std::vector<glm::vec2> _textureCoords{};

    glm::vec4 _colorDiffuse{};
    glm::vec3 _colorSpecular{};

    bool _hasTextureDiffuse{};
    bool _hasTextureSpecular{};


	unsigned _vbo{0};
    unsigned _ebo{0};
};