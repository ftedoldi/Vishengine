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
#include <unordered_map>
#include <unordered_set>

struct Particles {
    std::vector<glm::vec3> Positions{};
    std::vector<glm::vec3> OldPositions{};
    std::vector<glm::vec3> Velocities{};
    std::vector<float> InverseMasses{};
};

class Mesh {
public:
    using VertexIndex = uint32_t;
    using AdjacencyList = std::unordered_map<VertexIndex, std::unordered_set<VertexIndex>>;

    Mesh(Particles&& particles,
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

    Particles ParticlesData{};
    std::vector<DistanceConstraint> DistanceConstraints{};

    AdjacencyList _adjacencyList{};

    std::vector<uint32_t> Indices{};

    GLuint Vao{0};
    GLuint Vbo{0};

    std::vector<Texture> TexturesDiffuse{};
    std::vector<Texture> TexturesSpecular{};
    std::vector<Texture> TexturesNormal{};

private:
    void _initializeMesh();

    void _initializeConstraints();

    void _buildAdjacencyList();

    std::vector<glm::vec2> _textureCoords{};
    std::vector<glm::vec3> _normals{};

    glm::vec4 _colorDiffuse{};
    glm::vec3 _colorSpecular{};

    bool _hasTextureDiffuse{};
    bool _hasTextureSpecular{};

    GLuint _ebo{0};
};