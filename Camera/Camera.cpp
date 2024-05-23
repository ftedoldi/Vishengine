#include "Camera.h"

#include "GLFW/glfw3.h"

Camera::Camera(Window* window, const glm::vec3 position, const glm::vec3 up, const glm::vec3 direction) :
    _window(window), _position(position), _up(up), _direction(direction) {
    _transform.SetRotation(glm::quatLookAtRH(direction, up));
    _transform.SetTranslation(position);

    // Set framebuffer size changed event
    _window->OnFramebufferSizeChanged().AddFunction([this](const int width, const int height) { _onFramebufferSizeChanged(width, height); });

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
        _position += _direction * _cameraSpeed;
        _transform.SetTranslation(_position);
    }
    if (inputManager.isKeyPressed(GLFW_KEY_S)) {
        _position -= _direction * _cameraSpeed;
        _transform.SetTranslation(_position);
    }
    if (inputManager.isKeyPressed(GLFW_KEY_A)) {
        _position -= glm::normalize(glm::cross(_direction, _up)) * _cameraSpeed;
        _transform.SetTranslation(_position);
    }
    if (inputManager.isKeyPressed(GLFW_KEY_D)) {
        _position += glm::normalize(glm::cross(_direction, _up)) * _cameraSpeed;
        _transform.SetTranslation(_position);
    }
}

void Camera::_onFramebufferSizeChanged(const int width, const int height) {
    _perspectiveMatrix = glm::perspective(glm::radians(45.f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.f);
}
