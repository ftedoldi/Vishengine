#include "CameraMoveSystem.h"

CameraMoveSystem::CameraMoveSystem(entt::registry& registry, const entt::entity camera, InputManager* inputManager)
    : _registry{registry},  _inputManager{inputManager} {
    auto view{_registry.view<Camera, Transform>()};

    _camera = &view.get<Camera>(camera);
    _cameraTransform = &view.get<Transform>(camera);
}

void CameraMoveSystem::Update(const float deltaTime) {
    if(_inputManager->IsKeyPressed(GLFW_KEY_W)) {
        _camera->Position.y = _cameraYPosition;
        _camera->Position += _camera->Front * Camera::Speed * deltaTime;
        _cameraTransform->Translation = _camera->Position;
    }
    if(_inputManager->IsKeyPressed(GLFW_KEY_S)) {
        _camera->Position.y = _cameraYPosition;
        _camera->Position -= _camera->Front * Camera::Speed * deltaTime;
        _cameraTransform->Translation = _camera->Position;
    }
    if(_inputManager->IsKeyPressed(GLFW_KEY_A)) {
        _camera->Position -= glm::normalize(glm::cross(_camera->Front, _camera->Up)) * Camera::Speed * deltaTime;
        _cameraTransform->Translation = _camera->Position;
    }
    if(_inputManager->IsKeyPressed(GLFW_KEY_D)) {
        _camera->Position += glm::normalize(glm::cross(_camera->Front, _camera->Up)) * Camera::Speed * deltaTime;
        _cameraTransform->Translation = _camera->Position;
    }
    if(_inputManager->IsKeyPressed(GLFW_KEY_E)) {
        _camera->Position += glm::vec3{0, 1, 0} * Camera::Speed * deltaTime;
        _cameraTransform->Translation = _camera->Position;
        _cameraYPosition = _camera->Position.y;
    }
    if(_inputManager->IsKeyPressed(GLFW_KEY_Q)) {
        _camera->Position -= glm::vec3{0, 1, 0} * Camera::Speed * deltaTime;
        _cameraTransform->Translation = _camera->Position;
        _cameraYPosition = _camera->Position.y;
    }
}
