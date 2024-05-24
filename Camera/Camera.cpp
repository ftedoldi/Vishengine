#include "Camera.h"

#include "Time/Time.h"

#include "GLFW/glfw3.h"

Camera::Camera(Window* window, const glm::vec3 front, const glm::vec3 up, const glm::vec3 direction) :
    _window(window), _position(front), _up(up), _front(direction) {
    _transform.SetRotation(glm::quatLookAtRH(direction, up));
    _transform.SetTranslation(front);

    _right = glm::normalize(glm::cross(up, front));

    // Set framebuffer size changed event
    _window->OnFramebufferSizeChanged().AddFunction([this](const int width, const int height) { _onFramebufferSizeChanged(width, height); });

    _window->OnMouseMoved().AddFunction([this](const int xPos, const int yPos){ _onMouseMoved(xPos, yPos);});

    _perspectiveMatrix = glm::perspective(glm::radians(45.f), static_cast<float>(window->GetWidth()) / static_cast<float>(window->GetHeight()), 0.1f, 100.f);
}

void Camera::Start() {

}

void Camera::Update() {
}

glm::mat4 Camera::GetPerspectiveMatrix() const {
    return _perspectiveMatrix;
}

Transform Camera::GetTransform() const {
    return _transform;
}

void Camera::ProcessInput(InputManager& inputManager) {
    if (inputManager.isKeyPressed(GLFW_KEY_W)) {
        _position += _front * _cameraSpeed * Time::GetDeltaTime();
        _transform.SetTranslation(_position);
    }
    if (inputManager.isKeyPressed(GLFW_KEY_S)) {
        _position -= _front * _cameraSpeed * Time::GetDeltaTime();
        _transform.SetTranslation(_position);
    }
    if (inputManager.isKeyPressed(GLFW_KEY_A)) {
        _position -= glm::normalize(glm::cross(_front, _up)) * _cameraSpeed * Time::GetDeltaTime();
        _transform.SetTranslation(_position);
    }
    if (inputManager.isKeyPressed(GLFW_KEY_D)) {
        _position += glm::normalize(glm::cross(_front, _up)) * _cameraSpeed * Time::GetDeltaTime();
        _transform.SetTranslation(_position);
    }
}

void Camera::_onFramebufferSizeChanged(const int width, const int height) {
    _perspectiveMatrix = glm::perspective(glm::radians(45.f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.f);
}

void Camera::_onMouseMoved(const double xPos, const double yPos) {
    if (_firstTimeMovingMouse){
        _lastX = xPos;
        _lastY = yPos;
        _firstTimeMovingMouse = false;
    }

    double xOffset{xPos - _lastX};
    double yOffset{_lastY - yPos};

    _lastX = xPos;
    _lastY = yPos;

    double sensitivity{0.1f};
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    _cameraYaw   += xOffset;
    _cameraPitch += yOffset;

    if(_cameraPitch > 89.0)
        _cameraPitch = 89.0;
    if(_cameraPitch < -89.0)
        _cameraPitch = -89.0;

    glm::vec3 direction;
    direction.x = static_cast<float>(std::cos(glm::radians(_cameraYaw)) * std::cos(glm::radians(_cameraPitch)));
    direction.y = static_cast<float>(std::sin(glm::radians(_cameraPitch)));
    direction.z = static_cast<float>(std::sin(glm::radians(_cameraYaw)) * std::cos(glm::radians(_cameraPitch)));

    _front = glm::normalize(direction);
    _right = glm::normalize(glm::cross(_up, _front));
    _up = glm::normalize(glm::cross(_front, _right));

    _transform.SetRotation(glm::quatLookAtRH(_front, _up));
}
