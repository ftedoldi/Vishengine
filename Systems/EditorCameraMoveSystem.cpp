#include "EditorCameraMoveSystem.h"

#include "Components/Camera/ActiveCameraTag.h"
#include "Components/Camera/Camera.h"
#include "Components/Camera/EditorCameraTag.h"
#include "Components/Transforms/RelativeTransform.h"
#include "Components/Transforms/TransformDirtyFlag.h"
#include "Editor/ScenePanelInputState.h"

#include "glm/gtc/quaternion.hpp"

EditorCameraMoveSystem::EditorCameraMoveSystem(const std::shared_ptr<InputManager>& inputManager)
    : _inputManager{inputManager} {
}

void EditorCameraMoveSystem::Update(const float deltaTime, entt::registry& registry) {
    const glm::vec2 mouseDelta{_inputManager->GetMouseDelta()};
    const float rawScrollDelta{_inputManager->GetScrollDelta()};
    const bool rmbDown{_inputManager->IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)};
    const bool mmbDown{_inputManager->IsMouseButtonPressed(GLFW_MOUSE_BUTTON_MIDDLE)};

    const auto* const inputState{registry.ctx().find<ScenePanelInputState>()};
    const bool sceneHovered{inputState != nullptr && inputState->IsHovered};

    // Capture a drag if the button went down while the scene panel was hovered;
    // keep it captured until the button is released, even if the cursor leaves.
    if (!rmbDown) {
        _rmbCapturedByScene = false;
    } else if (!_rmbCapturedByScene && sceneHovered) {
        _rmbCapturedByScene = true;
    }
    if (!mmbDown) {
        _mmbCapturedByScene = false;
    } else if (!_mmbCapturedByScene && sceneHovered) {
        _mmbCapturedByScene = true;
    }

    const bool rmb{rmbDown && _rmbCapturedByScene};
    const bool mmb{mmbDown && _mmbCapturedByScene};
    const float scrollDelta{sceneHovered ? rawScrollDelta : 0.f};

    auto view{registry.view<Camera, EditorCameraTag, ActiveCameraTag, RelativeTransform>()};

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
            registry.emplace<TransformDirtyFlag>(entity);
        }
    }
}