#pragma once

#include "Camera/Camera.h"
#include "Components/Transform.h"
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

class Mesh {
public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    ~Mesh();

	std::vector<Vertex> Vertices{};
	std::vector<unsigned int> Indices{};

	unsigned Vbo{0};
    unsigned Vao{0};
    unsigned Ebo{0};
};