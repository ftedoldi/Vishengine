#pragma once

#include "Component.h"

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

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

private:
    void _setupMesh();

	std::vector<Vertex> _vertices{};
	std::vector<unsigned int> _indices{};

	unsigned int _vbo{0};
    unsigned int _vao{0};
    unsigned int _ebo{0};
};