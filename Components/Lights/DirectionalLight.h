#pragma once

#include <glm/glm.hpp>

struct DirectionalLight {
    glm::vec3 Diffuse{};
    glm::vec3 Specular{};
    glm::vec3 Ambient{};

    glm::vec3 Direction{};
};