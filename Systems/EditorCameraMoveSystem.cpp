#include "EditorCameraMoveSystem.h"

#include "Components/Camera/ActiveCameraTag.h"
#include "Components/Camera/Camera.h"
#include "Components/Camera/EditorCameraTag.h"
#include "Components/Transforms/RelativeTransform.h"
#include "Components/Transforms/TransformDirtyFlag.h"

EditorCameraMoveSystem::EditorCameraMoveSystem(const std::shared_ptr<InputManager>& inputManager)
    : _inputManager{inputManager} {
}

void EditorCameraMoveSystem::Update(const float deltaTime, entt::registry& registry) {
    auto view{registry.view<Camera, EditorCameraTag, ActiveCameraTag, RelativeTransform, TransformDirtyFlag>()};

    for(const auto entity: view) {
        const auto& cameraComponent{view.get<Camera>(entity)};
        auto& cameraPosition{view.get<RelativeTransform>(entity).Value.Position};

        bool cameraPositionChanged{};
        if(_inputManager->IsKeyPressed(GLFW_KEY_W)) {
            cameraPosition.y = _cameraYPosition;
            cameraPosition += cameraComponent.Front * cameraComponent.Speed * deltaTime;
            cameraPositionChanged = true;
        }
        if(_inputManager->IsKeyPressed(GLFW_KEY_S)) {
            cameraPosition.y = _cameraYPosition;
            cameraPosition -= cameraComponent.Front * cameraComponent.Speed * deltaTime;
            cameraPositionChanged = true;
        }
        if(_inputManager->IsKeyPressed(GLFW_KEY_A)) {
            cameraPosition -= glm::normalize(glm::cross(cameraComponent.Front, cameraComponent.Up)) * cameraComponent.Speed * deltaTime;
            cameraPositionChanged = true;
        }
        if(_inputManager->IsKeyPressed(GLFW_KEY_D)) {
            cameraPosition += glm::normalize(glm::cross(cameraComponent.Front, cameraComponent.Up)) * cameraComponent.Speed * deltaTime;
            cameraPositionChanged = true;
        }
        if(_inputManager->IsKeyPressed(GLFW_KEY_E)) {
            cameraPosition += glm::vec3{0, 1, 0} * cameraComponent.Speed * deltaTime;
            _cameraYPosition = cameraPosition.y;
            cameraPositionChanged = true;
        }
        if(_inputManager->IsKeyPressed(GLFW_KEY_Q)) {
            cameraPosition -= glm::vec3{0, 1, 0} * cameraComponent.Speed * deltaTime;
            _cameraYPosition = cameraPosition.y;
            cameraPositionChanged = true;
        }

        if (cameraPositionChanged) {
            auto& transformFlag{view.get<TransformDirtyFlag>(entity)};
            transformFlag.ShouldUpdateTransform = true;
        }
    }
}
