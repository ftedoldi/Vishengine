#include "DebugFramebufferPanel.h"
#include "Camera/CameraFactory.h"
#include "Components/RenderingComponents.h"

#include "imgui.h"

DebugFramebufferPanel::DebugFramebufferPanel(const FramebuffersController* const framebuffersController)
    : _framebuffersController{framebuffersController} {
    IsVisible = false;
}

void DebugFramebufferPanel::OnRender(entt::dispatcher& dispatcher, entt::registry& registry) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0.f, 0.f});


    // The panel was just opened.
    if (IsVisible && !_wasVisible) {
        // Create an entity with a shader and a framebuffer used to debug the frustum.
        // Also on these framebuffer both the colliders and the meshes need to be drawn.
        _debugViewEntity = CameraFactory::CreateGameCamera(
            registry,
            glm::vec3{0., 0., 30.},
            45.,
            1.f,
            0.1,
            300.,
            CameraType::Perspective);

        registry.emplace<RenderTarget>(_debugViewEntity, FramebufferID::FrustumDebugView);
        registry.emplace<RenderPass>(_debugViewEntity, ShaderID::FrustumDebug);
        std::bitset<32> layers{};
        layers.set(static_cast<size_t>(RenderLayer::SceneMeshes));
        layers.set(static_cast<size_t>(RenderLayer::DebugFrustumIntersections));
        registry.emplace<RenderLayers>(_debugViewEntity, layers);
    }

    // The panel was just closed.
    if (!IsVisible && _wasVisible) {
        if (registry.valid(_debugViewEntity)) {
            registry.destroy(_debugViewEntity);
        }
        _debugViewEntity = entt::null;
    }

    _wasVisible = IsVisible;

    if (!IsVisible) {
        ImGui::PopStyleVar();
        return;
    }

    constexpr ImGuiWindowFlags flags{
        ImGuiWindowFlags_NoCollapse        |
        ImGuiWindowFlags_NoScrollbar       |
        ImGuiWindowFlags_NoScrollWithMouse };

    ImGui::Begin("Debug Framebuffer", &IsVisible, flags);

    const auto regionSize{ImGui::GetContentRegionAvail()};

    const auto* const framebuffer{_framebuffersController->GetFramebuffer(FramebufferID::FrustumDebugView)};
    ImGui::Image(
        framebuffer->GetColorAttachmentID(),
        regionSize,
        ImVec2{0.f, 1.f},
        ImVec2{1.f, 0.f});

    ImGui::End();
    ImGui::PopStyleVar();
}