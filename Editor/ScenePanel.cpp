#include "ScenePanel.h"
#include "Events/ScenePanelEvents.h"

#include "imgui.h"

ScenePanel::ScenePanel(std::shared_ptr<Framebuffer> framebuffer)
    : _framebuffer{std::move(framebuffer)} {}

void ScenePanel::OnRender(entt::dispatcher& dispatcher, entt::registry& /*registry*/) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0.f, 0.f});

    constexpr ImGuiWindowFlags flags{
        ImGuiWindowFlags_NoCollapse        |
        ImGuiWindowFlags_NoScrollbar       |
        ImGuiWindowFlags_NoScrollWithMouse };

    ImGui::Begin("Scene", nullptr, flags);

    if (const auto contentPos{ImGui::GetCursorScreenPos()}; _lastMousePosition.x != contentPos.x || _lastMousePosition.y != contentPos.y) {
        const glm::vec2 scenePanelMousePosition{contentPos.x, contentPos.y};
        dispatcher.trigger<ScenePanelEvents::ScenePanelMouseMovedEvent>({scenePanelMousePosition});
        _lastMousePosition = contentPos;
    }

    const auto regionSize{ImGui::GetContentRegionAvail()};
    if (_lastSize.x != regionSize.x || _lastSize.y != regionSize.y) {
        const glm::vec2 scenePanelSize{regionSize.x, regionSize.y};
        dispatcher.trigger<ScenePanelEvents::ScenePanelResizedEvent>({scenePanelSize});
        _lastSize = regionSize;
    }

    // Blit framebuffer color attachment as an ImGui image.
    const auto textureID{_framebuffer->GetColorAttachmentID()};
    ImGui::Image(
        reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(textureID)),
        regionSize,
        ImVec2{0.f, 1.f}, // UV top-left
        ImVec2{1.f, 0.f});  // UV bot-right

    ImGui::End();
    ImGui::PopStyleVar();
}
