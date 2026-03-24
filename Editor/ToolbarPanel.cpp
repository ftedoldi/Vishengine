#include "ToolbarPanel.h"

#include "imgui.h"

void ToolbarPanel::OnRender(entt::registry& /*registry*/) {
    // Render as a slim, full-width window pinned to the top of the editor layout
    constexpr ImGuiWindowFlags toolbarFlags{
        ImGuiWindowFlags_NoScrollbar       |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoCollapse        |
        ImGuiWindowFlags_NoTitleBar        };


    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{4.f, 4.f});
    ImGui::Begin("##Toolbar", nullptr, toolbarFlags);

    const float windowWidth{ImGui::GetWindowWidth()};
    constexpr float buttonWidth{32.f};
    constexpr float spacing{4.f};
    constexpr float totalWidth{buttonWidth * 3.f + spacing * 2.f};

    ImGui::SetCursorPosX((windowWidth - totalWidth) * 0.5f);

    _renderPlayButton(buttonWidth);

    ImGui::SameLine(0.f, spacing);

    _renderPauseButton(buttonWidth);

    ImGui::SameLine(0.f, spacing);

    _renderStopButton(buttonWidth);

    ImGui::End();
    ImGui::PopStyleVar();
}

void ToolbarPanel::_renderPlayButton(const float buttonWidth) {
    ImGui::PushStyleColor(ImGuiCol_Button, _isPlaying ? _untoggledColor : ImVec4{0.1f, 0.7f, 0.1f, 1.f});

    if (ImGui::Button("  >  ", ImVec2{buttonWidth, 0.f})) {
        // TODO: add logic for scene play
        _isPlaying = !_isPlaying;
        if (_isPlaying) {
            _isPaused = false;
        }
    }

    ImGui::PopStyleColor();

    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Play");
    }
}

void ToolbarPanel::_renderPauseButton(const float buttonWidth) {
    ImGui::PushStyleColor(ImGuiCol_Button, _isPaused ? ImVec4{0.8f, 0.7f, 0.1f, 1.f} : _untoggledColor);

    if (ImGui::Button(" || ", ImVec2{buttonWidth, 0.f})) {
        // TODO: add logic for scene pause
        if (_isPlaying) {
            _isPaused = !_isPaused;
        }
    }
    ImGui::PopStyleColor();

    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Pause");
    }
}

void ToolbarPanel::_renderStopButton(const float buttonWidth) {
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.15f, 0.15f, 1.f});
    if (ImGui::Button("  []  ", ImVec2{buttonWidth, 0.f})) {
        // TODO: add logic for scene stop
        _isPlaying = false;
        _isPaused  = false;
    }
    ImGui::PopStyleColor();

    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Stop");
    }
}
