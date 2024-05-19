#pragma once

#include "EntityComponent/Component.h"
#include "Math/Transform.h"
#include "Window/Window.h"

#include <glm/mat4x4.hpp>

#include <GLFW/glfw3.h>

class Camera : public Component
{
public:
    explicit Camera(Window* window,
                    glm::vec3 position = glm::vec3{0.f, 0.f, 0.f},
                    glm::vec3 up = glm::vec3{0.f, 1.f, 0.f},
                    glm::vec3 direction = glm::vec3{0.f, 0.f, -1.f});

    void Start() override;

    void Update() override;

    void OnMovementKeyPressed(int key, int action);

    // TODO: change the scope of this matrix
    static glm::mat4 PerspectiveMatrix;

private:
    Window* _window;
    Transform _transform;
    glm::vec3 _position{};
    glm::vec3 _direction{};
    glm::vec3 _up{};
};