#pragma once

#include "Components/CameraComponents/Camera.h"
#include "Components/Transform.h"
#include "Shaders/Shader.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <iostream>
#include <vector>
#include <memory>

class Mesh {
public:
    Mesh() = default;
    Mesh(std::vector<glm::vec3> positions, std::vector<glm::vec2> textureCoords, std::vector<unsigned int> indices = {});
    ~Mesh();

    std::vector<glm::vec3> Vertices{};
    std::vector<glm::vec2> TextureCoords{};
	std::vector<unsigned int> Indices{};

	unsigned Vbo{0};
    unsigned Vao{0};
    unsigned Ebo{0};
};