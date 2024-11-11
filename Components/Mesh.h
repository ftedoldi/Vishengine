#pragma once

#include "Components/CameraComponents/Camera.h"

#include "Physics/DistanceConstraint.h"
#include "Shaders/Shader.h"
#include "Texture/Texture.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <iostream>
#include <vector>
#include <memory>

struct PointsMass {
    std::vector<glm::vec3> Positions{};
    std::vector<glm::vec3> OldPositions{};
    std::vector<glm::vec3> Velocities{};
    std::vector<float> Masses{};
    std::vector<float> InverseMasses{};
};

class Mesh {
public:
    Mesh(PointsMass&& vertices,
         std::vector<glm::vec2> textureCoords,
         std::vector<unsigned int> indices,
         std::vector<glm::vec3> normals);

    ~Mesh();

    void SetColorDiffuse(glm::vec4 colorDiffuse);
    glm::vec4 GetColorDiffuse() const;

    void SetColorSpecular(glm::vec3 colorSpecular);
    glm::vec3 GetColorSpecular() const;

    void SetHasTextureDiffuse(bool hasTextureDiffuse);
    bool GetHasTextureDiffuse() const;

    void SetHasTextureSpecular(bool hasTextureSpecular);
    bool GetHasTextureSpecular() const;

    PointsMass PointsMasses{};
    std::vector<DistanceConstraint> DistanceConstraints{};

    std::vector<unsigned int> Indices{};

    GLuint Vao{0};
    GLuint Vbo{0};

    std::vector<Texture> TexturesDiffuse{};
    std::vector<Texture> TexturesSpecular{};
    std::vector<Texture> TexturesNormal{};

private:
    void _initializeMesh();
    void _initializeConstraints();

    std::vector<glm::vec2> _textureCoords{};
    std::vector<glm::vec3> _normals{};

    glm::vec4 _colorDiffuse{};
    glm::vec3 _colorSpecular{};

    bool _hasTextureDiffuse{};
    bool _hasTextureSpecular{};

    GLuint _ebo{0};
};