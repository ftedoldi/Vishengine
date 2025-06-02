#pragma once

#include "glm/mat4x4.hpp"

#include "GLFW/glfw3.h"

class Camera {
public:
    explicit Camera(glm::vec3 up = glm::vec3{0., 1., 0.},
                    glm::vec3 front = glm::vec3{0., 0., -1.});

    glm::vec3 Front{};
    glm::vec3 Up{};

    float Speed{2.5f};

    float YawAngle{};
    float PitchAngle{};

    float FOV{};
    float AspectRatio{};
    float NearPlaneZDistance{};
    float FarPlaneZDistance{};

    glm::mat4 ProjectionMatrix{};
};