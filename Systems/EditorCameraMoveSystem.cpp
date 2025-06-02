#include "EditorCameraMoveSystem.h"
#include "Components/Camera/ActiveCameraTag.h"
#include "Components/Camera/EditorCameraTag.h"

EditorCameraMoveSystem::EditorCameraMoveSystem(InputManager* const inputManager)
    : _inputManager{inputManager} {
}

void EditorCameraMoveSystem::Update(const float deltaTime, entt::registry& registry) {
    auto view{registry.view<Camera, EditorCameraTag, ActiveCameraTag, Position>()};

    for(const auto entity: view) {
        const auto& cameraComponent{view.get<Camera>(entity)};
        auto& cameraPosition{view.get<Position>(entity)};

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
}
