#pragma once

#include <glm/glm.hpp>

struct PointLight {
    glm::vec3 Diffuse{};
    glm::vec3 Specular{};
    glm::vec3 Ambient{};

    float Constant{};
    float Linear{};
    float Quadratic{};
};
