#include "ScenePanel.h"

#include "imgui.h"

ScenePanel::ScenePanel(std::shared_ptr<Framebuffer> framebuffer)
    : _framebuffer{std::move(framebuffer)} {}

void ScenePanel::OnRender(entt::registry& /*registry*/) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0.f, 0.f});

    constexpr ImGuiWindowFlags flags{
        ImGuiWindowFlags_NoCollapse        |
        ImGuiWindowFlags_NoScrollbar       |
        ImGuiWindowFlags_NoScrollWithMouse };

    ImGui::Begin("Scene", nullptr, flags);

    const ImVec2 viewportSize{ImGui::GetContentRegionAvail()};

    // Blit framebuffer color attachment as an ImGui image.
    const auto textureID{_framebuffer->GetColorAttachmentID()};
    ImGui::Image(
        reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(textureID)),
        viewportSize,
        ImVec2{0.f, 1.f}, // UV top-left
        ImVec2{1.f, 0.f});  // UV bot-right

    ImGui::End();
    ImGui::PopStyleVar();
}

float ScenePanel::GetViewportWidth() const {
    return _viewportWidth;
}

float ScenePanel::GetViewportHeight() const {
    return _viewportHeight;
}
