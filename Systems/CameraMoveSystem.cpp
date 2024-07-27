#include "CameraMoveSystem.h"

CameraMoveSystem::CameraMoveSystem(entt::registry& registry, const entt::entity camera, InputManager* inputManager)
    : _registry{registry},  _inputManager{inputManager} {

    _camera = &_registry.get<Camera>(camera);
    _position = &_registry.get<Position>(camera);
    _rotation = &_registry.get<Rotation>(camera);
}

void CameraMoveSystem::Update(const float deltaTime) {
    if(_inputManager->IsKeyPressed(GLFW_KEY_W)) {
        _position->Vector.y = _cameraYPosition;
        _position->Vector += _camera->Front * Camera::Speed * deltaTime;
    }
    if(_inputManager->IsKeyPressed(GLFW_KEY_S)) {
        _position->Vector.y = _cameraYPosition;
        _position->Vector -= _camera->Front * Camera::Speed * deltaTime;
    }
    if(_inputManager->IsKeyPressed(GLFW_KEY_A)) {
        _position->Vector -= glm::normalize(glm::cross(_camera->Front, _camera->Up)) * Camera::Speed * deltaTime;
    }
    if(_inputManager->IsKeyPressed(GLFW_KEY_D)) {
        _position->Vector += glm::normalize(glm::cross(_camera->Front, _camera->Up)) * Camera::Speed * deltaTime;
    }
    if(_inputManager->IsKeyPressed(GLFW_KEY_E)) {
        _position->Vector += glm::vec3{0, 1, 0} * Camera::Speed * deltaTime;
        _cameraYPosition = _position->Vector.y;
    }
    if(_inputManager->IsKeyPressed(GLFW_KEY_Q)) {
        _position->Vector -= glm::vec3{0, 1, 0} * Camera::Speed * deltaTime;
        _cameraYPosition = _position->Vector.y;
    }
}
