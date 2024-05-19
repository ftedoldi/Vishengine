#include "Camera.h"

glm::mat4 Camera::PerspectiveMatrix;

Camera::Camera(Window* window, const glm::vec3 position, const glm::vec3 up, const glm::vec3 direction) :
    _window(window), _position(position), _up(up), _direction(direction) {
    _transform.SetRotation(glm::quatLookAtRH(direction, up));
    _transform.SetTranslation(position);

    _window->OnKeyboardKeyPressed().AddFunction([this](int key, int action) { OnMovementKeyPressed(key, action); });
}

void Camera::OnMovementKeyPressed(const int key, const int action) {
    if (key == GLFW_KEY_W && action == GLFW_PRESS) {
        std::cout << "W pressed!!" << std::endl;
        _position += _direction;
        _transform.SetTranslation(_position);
    }
}

void Camera::Start() {
}

void Camera::Update() {
    _transform.SetRotation(glm::quatLookAtRH(_direction, _up));
    _transform.SetTranslation(_position);
}