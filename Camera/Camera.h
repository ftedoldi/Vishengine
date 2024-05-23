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

    glm::mat4 GetPerspectiveMatrix() const;

    Transform GetTransform() const;

    void ProcessInput(InputManager& inputManager);

private:
    void _onMovementKeyPressed(GLFWwindow* window, int key, int action);

    void _onFramebufferSizeChanged(int width, int height);

    Window* _window;

    Transform _transform;

    glm::vec3 _position{};
    glm::vec3 _direction{};
    glm::vec3 _up{};

    glm::mat4 _perspectiveMatrix{};

    static inline const float _cameraSpeed{0.05f};
};