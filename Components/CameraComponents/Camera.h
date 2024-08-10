#pragma once

#include "glm/mat4x4.hpp"

#include "GLFW/glfw3.h"

enum class CameraType {
    Perspective,
    Orthogonal,
};

class Camera {
public:
    explicit Camera(glm::vec3 up = glm::vec3{0.f, 1.f, 0.f},
                    glm::vec3 front = glm::vec3{0.f, 0.f, -1.f});

    glm::vec3 Front{};
    glm::vec3 Up{};

    static inline const float Speed{2.5f};

    double YawAngle{0.0};
    double PitchAngle{0.0};
};