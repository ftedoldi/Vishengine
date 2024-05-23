#pragma once

#include "Component.h"

#include "Camera/Camera.h"
#include "Math/Transform.h"
#include "Shaders/Shader.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <iostream>
#include <vector>
#include <memory>

struct Vertex {
    Vertex(glm::vec3 position, glm::vec2 textureCoord) : Position(position), TextureCoord(textureCoord){}

    glm::vec3 Position;
    glm::vec2 TextureCoord;
};

class Mesh : public Component {
public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    ~Mesh() override;

    void Start() override;
    void Update() override;

    Camera* _camera{nullptr};
    Shader* _shader{nullptr};

private:
    void _setupMesh();
    void _updateMesh();

	std::vector<Vertex> _vertices{};
	std::vector<unsigned int> _indices{};

	unsigned int _vbo{0};
    unsigned int _vao{0};
    unsigned int _ebo{0};

    Transform _transform;
};