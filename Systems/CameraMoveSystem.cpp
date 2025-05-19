#include "CameraMoveSystem.h"

CameraMoveSystem::CameraMoveSystem(entt::registry& registry, const entt::entity camera, InputManager* const inputManager)
    : _registry{registry},  _inputManager{inputManager}, _camera{camera} {
}

void CameraMoveSystem::Update(const float deltaTime) {
    const auto& cameraComponent{_registry.get<Camera>(_camera)};
    auto& cameraPosition{_registry.get<Position>(_camera)};
    
    if(_inputManager->IsKeyPressed(GLFW_KEY_W)) {
        cameraPosition.Vector.y = _cameraYPosition;
        cameraPosition.Vector += cameraComponent.Front * cameraComponent.Speed * deltaTime;
    }
    if(_inputManager->IsKeyPressed(GLFW_KEY_S)) {
        cameraPosition.Vector.y = _cameraYPosition;
        cameraPosition.Vector -= cameraComponent.Front * cameraComponent.Speed * deltaTime;
    }
    if(_inputManager->IsKeyPressed(GLFW_KEY_A)) {
        cameraPosition.Vector -= glm::normalize(glm::cross(cameraComponent.Front, cameraComponent.Up)) * cameraComponent.Speed * deltaTime;
    }
    if(_inputManager->IsKeyPressed(GLFW_KEY_D)) {
        cameraPosition.Vector += glm::normalize(glm::cross(cameraComponent.Front, cameraComponent.Up)) * cameraComponent.Speed * deltaTime;
    }
    if(_inputManager->IsKeyPressed(GLFW_KEY_E)) {
        cameraPosition.Vector += glm::vec3{0, 1, 0} * cameraComponent.Speed * deltaTime;
        _cameraYPosition = cameraPosition.Vector.y;
    }
    if(_inputManager->IsKeyPressed(GLFW_KEY_Q)) {
        cameraPosition.Vector -= glm::vec3{0, 1, 0} * cameraComponent.Speed * deltaTime;
        _cameraYPosition = cameraPosition.Vector.y;
    }
}
