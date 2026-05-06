#include "ScenePanel.h"
#include "Events/ScenePanelEvents.h"
#include "ScenePanelInputState.h"

#include "Components/Camera/Camera.h"
#include "Components/Camera/EditorCameraTag.h"
#include "Components/SelectedTag.h"
#include "Components/Relationship.h"
#include "Components/Transforms/RelativeTransform.h"
#include "Components/Transforms/TransformDirtyFlag.h"
#include "Components/Transforms/WorldTransform.h"

#include "Components/RenderingComponents.h"
#include "ImGuizmo.h"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"

ScenePanel::ScenePanel(const FramebuffersController* const framebuffersController)
    : _framebuffersController{framebuffersController} {}

void ScenePanel::OnRender(entt::dispatcher& dispatcher, entt::registry& registry) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0.f, 0.f});

    constexpr ImGuiWindowFlags flags{
        ImGuiWindowFlags_NoCollapse        |
        ImGuiWindowFlags_NoScrollbar       |
        ImGuiWindowFlags_NoScrollWithMouse };

    ImGui::Begin("Scene", nullptr, flags);

    const auto contentPos{ImGui::GetCursorScreenPos()};
    if (contentPos.x != _panelPos.x || contentPos.y != _panelPos.y) {
        dispatcher.trigger<ScenePanelEvents::ScenePanelMouseMovedEvent>({glm::vec2{contentPos.x, contentPos.y}});
        _panelPos = contentPos;
    }

    const auto regionSize{ImGui::GetContentRegionAvail()};
    if (_lastSize.x != regionSize.x || _lastSize.y != regionSize.y) {
        dispatcher.trigger<ScenePanelEvents::ScenePanelResizedEvent>({glm::vec2{regionSize.x, regionSize.y}});
        _lastSize = regionSize;
    }

    const auto mainView{registry.view<MainViewTag, RenderTarget, RenderPass>()};

    mainView.each([this, regionSize, &registry, contentPos](const entt::entity, const RenderTarget renderTarget, const RenderPass renderPass) {
        const auto framebuffer{_framebuffersController->GetFramebuffer(renderTarget.FramebufferHandle)};
        ImGui::Image(
            framebuffer->GetColorAttachmentID(),
            regionSize,
        ImVec2{0.f, 1.f},
        ImVec2{1.f, 0.f});

        auto& inputState{registry.ctx().emplace<ScenePanelInputState>()};
        inputState.IsHovered = ImGui::IsItemHovered();

        _drawGizmo(contentPos, regionSize, registry);
    });


    ImGui::End();
    ImGui::PopStyleVar();
}

void ScenePanel::_drawGizmo(const ImVec2 panelPos, const ImVec2 panelSize, entt::registry& registry) const {
    // TODO: improve this code.
    // Find selected entity that has a world transform.
    const auto selectedView{registry.view<SelectedTag, WorldTransform>()};
    if (selectedView.begin() == selectedView.end()) return;

    const auto selectedEntity{*selectedView.begin()};
    const auto& worldTransform{registry.get<WorldTransform>(selectedEntity).Value};

    // Get editor camera.
    const auto cameraView{registry.view<Camera, EditorCameraTag>()};
    if (cameraView.begin() == cameraView.end()) return;

    const auto& camera{cameraView.get<Camera>(*cameraView.begin())};

    // Build view matrix from the camera's ViewTransform (which is world.Invert()).
    glm::mat4 viewMat{glm::mat4_cast(camera.ViewTransform.Rotation)};
    viewMat[3] = glm::vec4{camera.ViewTransform.Position, 1.f};

    // Build world matrix for the selected entity: T * R * S.
    const glm::mat4 worldMat{
        glm::translate(glm::mat4{1.f}, worldTransform.Position) *
        glm::mat4_cast(worldTransform.Rotation) *
        glm::scale(glm::mat4{1.f}, glm::vec3{worldTransform.Scale})};

    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
    ImGuizmo::SetRect(panelPos.x, panelPos.y, panelSize.x, panelSize.y);

    glm::mat4 manipulated{worldMat};
    const bool changed{ImGuizmo::Manipulate(
        glm::value_ptr(viewMat),
        glm::value_ptr(camera.ProjectionMatrix),
        ImGuizmo::TRANSLATE,
        ImGuizmo::WORLD,
        glm::value_ptr(manipulated))};

    if (!changed) return;

    // Decompose the new world matrix.

    float newPosition[3]{};
    float newScale[3]{};
    float newRotation[3]{};
    ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(manipulated), newPosition, newRotation, newScale);

    const glm::quat newWorldQuat{glm::radians(glm::vec3{newRotation[0], newRotation[1], newRotation[2]})};
    const Transform newWorldTransform{{newPosition[0], newPosition[1], newPosition[2]}, newWorldQuat, newScale[0]};

    // Factor out parent world transform to get the new relative transform.
    auto& relTransform{registry.get<RelativeTransform>(selectedEntity).Value};
    const auto* rel{registry.try_get<Relationship>(selectedEntity)};
    if (rel && rel->Parent != entt::null) {
        const auto& parentWorld{registry.get<WorldTransform>(rel->Parent).Value};
        relTransform = parentWorld.Invert().Cumulate(newWorldTransform);
    } else {
        relTransform = newWorldTransform;
    }

    if (auto* dirty{registry.try_get<TransformDirtyFlag>(selectedEntity)}) {
        dirty->ShouldUpdateTransform = true;
    }
}