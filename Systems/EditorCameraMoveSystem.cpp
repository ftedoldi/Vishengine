#include "EditorCameraMoveSystem.h"

#include "Components/Camera/ActiveCameraTag.h"
#include "Components/Camera/Camera.h"
#include "Components/Camera/EditorCameraTag.h"
#include "Components/Transforms/RelativeTransform.h"
#include "Components/Transforms/TransformDirtyFlag.h"

#include "glm/gtc/quaternion.hpp"

EditorCameraMoveSystem::EditorCameraMoveSystem(const std::shared_ptr<InputManager>& inputManager)
    : _inputManager{inputManager} {
}

void EditorCameraMoveSystem::Update(const float deltaTime, entt::registry& registry) const {
    const glm::vec2 mouseDelta{_inputManager->GetMouseDelta()};
    const float scrollDelta{_inputManager->GetScrollDelta()};
    const bool rmb{_inputManager->IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)};
    const bool mmb{_inputManager->IsMouseButtonPressed(GLFW_MOUSE_BUTTON_MIDDLE)};

    auto view{registry.view<Camera, EditorCameraTag, ActiveCameraTag, RelativeTransform, TransformDirtyFlag>()};

    for (const auto entity : view) {
        auto& camera{view.get<Camera>(entity)};
        auto& relTransform{view.get<RelativeTransform>(entity).Value};
        bool changed{false};

        // RMB held: mouse look + WASD fly-through
        if (rmb) {
            constexpr float kSensitivity{0.15f};
            camera.YawAngle += mouseDelta.x * kSensitivity;
            camera.PitchAngle -= mouseDelta.y * kSensitivity;
            camera.PitchAngle = glm::clamp(camera.PitchAngle, -89.f, 89.f);

            const float yawRad{glm::radians(camera.YawAngle)};
            const float pitchRad{glm::radians(camera.PitchAngle)};
            const glm::quat yawQ{glm::angleAxis(-yawRad, glm::vec3{0.f, 1.f, 0.f})};
            const glm::quat pitchQ{glm::angleAxis(pitchRad, glm::vec3{1.f, 0.f, 0.f})};
            relTransform.Rotation = yawQ * pitchQ;
            camera.Front = glm::normalize(relTransform.Rotation * glm::vec3{0.f, 0.f, -1.f});

            const glm::vec3 right{glm::normalize(glm::cross(camera.Front, camera.Up))};
            if (_inputManager->IsKeyPressed(GLFW_KEY_W)) {
                relTransform.Position += camera.Front * camera.Speed * deltaTime;
            }
            if (_inputManager->IsKeyPressed(GLFW_KEY_S)) {
                relTransform.Position -= camera.Front * camera.Speed * deltaTime;
            }
            if (_inputManager->IsKeyPressed(GLFW_KEY_A)) {
                relTransform.Position -= right * camera.Speed * deltaTime;
            }
            if (_inputManager->IsKeyPressed(GLFW_KEY_D)) {
                relTransform.Position += right * camera.Speed * deltaTime;
            }
            if (_inputManager->IsKeyPressed(GLFW_KEY_E)) {
                relTransform.Position += camera.Up * camera.Speed * deltaTime;
            }
            if (_inputManager->IsKeyPressed(GLFW_KEY_Q)) {
                relTransform.Position -= camera.Up * camera.Speed * deltaTime;
            }

            changed = true;
        }

        // MMB held: pan (strafe + vertical)
        if (mmb) {
            const float panSpeed{camera.Speed * 0.005f};
            const glm::vec3 right{glm::normalize(glm::cross(camera.Front, camera.Up))};
            relTransform.Position -= right     * mouseDelta.x * panSpeed;
            relTransform.Position += camera.Up * mouseDelta.y * panSpeed;
            changed = true;
        }

        // Scroll: dolly along view direction
        if (scrollDelta != 0.f) {
            relTransform.Position += camera.Front * scrollDelta * camera.Speed * 0.5f;
            changed = true;
        }

        if (changed) {
            view.get<TransformDirtyFlag>(entity).ShouldUpdateTransform = true;
        }
    }
}