#pragma once

#include "Components/Transform.h"

#include <glm/mat4x4.hpp>

#include <GLFW/glfw3.h>

// Editor camera
class Camera
{
public:
    explicit Camera(glm::vec3 front = glm::vec3{0.f, 0.f, 0.f},
                    glm::vec3 up = glm::vec3{0.f, 1.f, 0.f},
                    glm::vec3 direction = glm::vec3{0.f, 0.f, -1.f});

    glm::vec3 Position{};
    glm::vec3 Front{};
    glm::vec3 Up{};

    static inline const float CameraSpeed{10.f};

    glm::mat4 PerspectiveMatrix{};

    bool FirstTimeMovingMouse{true};

    double LastX{0.0};
    double LastY{0.0};

    double CameraYaw{0.0};
    double CameraPitch{0.0};
};