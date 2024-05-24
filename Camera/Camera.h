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
                    glm::vec3 front = glm::vec3{0.f, 0.f, 0.f},
                    glm::vec3 up = glm::vec3{0.f, 1.f, 0.f},
                    glm::vec3 direction = glm::vec3{0.f, 0.f, -1.f});

    void Start() override;

    void Update() override;

    glm::mat4 GetPerspectiveMatrix() const;

    Transform GetTransform() const;

    void ProcessInput(InputManager& inputManager);

private:
    void _onFramebufferSizeChanged(int width, int height);
    void _onMouseMoved(double xPos, double yPos);

    Window* _window;

    Transform _transform;

    glm::vec3 _position{};
    glm::vec3 _front{};
    glm::vec3 _up{};
    glm::vec3 _right{};

    glm::mat4 _perspectiveMatrix{};

    bool _firstTimeMovingMouse{true};

    double _lastX = 400.0;
    double _lastY = 300.0;

    double _cameraYaw{-90.0};
    double _cameraPitch{0.0};

    static inline const float _cameraSpeed{2.5f};
};