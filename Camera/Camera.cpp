#include "Camera.h"

#include "Time/Time.h"

#include "GLFW/glfw3.h"

Camera::Camera(Window* window, const glm::vec3 position, const glm::vec3 up, const glm::vec3 front) :
    _window(window), _position(position), _up(up), _front(front) {
    _transform.SetRotation(glm::quatLookAtRH(front, up));
    _transform.SetTranslation(position);

    _right = glm::normalize(glm::cross(front, up));

    // Set framebuffer size changed event
    _window->OnFramebufferSizeChanged().AddFunction([this](const int width, const int height) { _onFramebufferSizeChanged(width, height); });

    _window->OnMouseMoved().AddFunction([this](const double xPos, const double yPos){ _onMouseMoved(xPos, yPos);});

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

void Camera::ProcessInput(InputManager inputManager) {
    if (inputManager.isKeyPressed(GLFW_KEY_W)) {
        _position.y = _cameraYPosition;
        _position += _front * _cameraSpeed * Time::GetDeltaTime();
        _transform.SetTranslation(_position);
    }
    if (inputManager.isKeyPressed(GLFW_KEY_S)) {
        _position.y = _cameraYPosition;
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
    if (inputManager.isKeyPressed(GLFW_KEY_E)) {
        _position += glm::vec3{0, 1, 0} * _cameraSpeed * Time::GetDeltaTime();
        _transform.SetTranslation(_position);
        _cameraYPosition = _position.y;
    }
    if (inputManager.isKeyPressed(GLFW_KEY_Q)) {
        _position -= glm::vec3{0, 1, 0} * _cameraSpeed * Time::GetDeltaTime();
        _transform.SetTranslation(_position);
        _cameraYPosition = _position.y;
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

    const float sensitivity{0.1f};
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    _cameraYaw   += xOffset;
    _cameraPitch += yOffset;

    if(_cameraPitch > 89.0)
        _cameraPitch = 89.0;
    if(_cameraPitch < -89.0)
        _cameraPitch = -89.0;

    const auto qx{glm::angleAxis(static_cast<float>(glm::radians(_cameraPitch / 2.f)), glm::vec3{1.0, 0.0, 0.0})};
    const auto qy{glm::angleAxis(static_cast<float>(glm::radians(-_cameraYaw / 2.f)), glm::vec3{0.0, 1.0, 0.0})};
    const auto q{qy * qx};

    _front = glm::normalize(q * glm::vec3{0.0, 0.0, -1.0});
    _right = glm::normalize(q * glm::vec3{1.0, 0.0, 0.0});
    _up = glm::normalize(q * glm::vec3{0.0, 1.0, 0.0});

    _transform.SetRotation(glm::quatLookAtRH(_front, _up));
}
