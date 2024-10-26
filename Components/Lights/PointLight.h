#pragma once

#include <glm/glm.hpp>

struct PointLight {
    glm::vec3 Diffuse{};
    glm::vec3 Specular{};
    glm::vec3 Ambient{};

    float Constant{1.f};
    float Linear{0.09f};
    float Quadratic{0.032f};
};
