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
    Mesh(std::vector<glm::vec3> vertices, std::vector<glm::vec2> textureCoords, std::vector<unsigned int> indices = {});
    ~Mesh();

    std::vector<unsigned int> Indices{};
    unsigned Vao{0};

private:
    std::vector<glm::vec3> _vertices{};
    std::vector<glm::vec2> _textureCoords{};

	unsigned _vbo{0};
    unsigned _ebo{0};
};