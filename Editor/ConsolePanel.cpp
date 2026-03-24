#include "ConsolePanel.h"

#include "imgui.h"

void ConsolePanel::Log(const LogLevel level, const std::string& message) {
    _entries.push_back({level, message});
    _scrollToBottom = true;
}

void ConsolePanel::OnRender(entt::registry& /*registry*/) {
    constexpr ImGuiWindowFlags flags{
        ImGuiWindowFlags_NoCollapse};

    ImGui::Begin("Console", nullptr, flags);

    constexpr ImVec4 untoggledColor{.3f, .3f, .3f, 1.f};
    ImGui::PushStyleColor(ImGuiCol_Button, _showInfo ? ImVec4{0.2f, 0.6f, 0.9f, 1.f} : untoggledColor);
    if (ImGui::SmallButton("Info")) {
        _showInfo = !_showInfo;
    }
    ImGui::PopStyleColor();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, _showWarnings ? ImVec4{0.9f, 0.7f, 0.1f, 1.f} : untoggledColor);
    if (ImGui::SmallButton("Warnings")) {
        _showWarnings = !_showWarnings;
    }
    ImGui::PopStyleColor();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button,_showErrors ? ImVec4{0.9f, 0.2f, 0.2f, 1.f} : untoggledColor);
    if (ImGui::SmallButton("Errors")) {
        _showErrors = !_showErrors;
    }
    ImGui::PopStyleColor();
    ImGui::SameLine();

    if (ImGui::SmallButton("Clear")) {
        Clear();
    }

    ImGui::Separator();

    // Scrollable log area
    ImGui::BeginChild("##ConsoleScrollArea",
                      ImVec2{0.f, 0.f},
                      false,
                      ImGuiWindowFlags_HorizontalScrollbar);

    for (const auto& entry : _entries) {
        if (entry.level == LogLevel::Info && !_showInfo) {
            continue;
        }

        if (entry.level == LogLevel::Warning && !_showWarnings) {
            continue;
        }

        if (entry.level == LogLevel::Error   && !_showErrors) {
            continue;
        }

        ImVec4 color{1.f, 1.f, 1.f, 1.f};
        const char* prefix{"[INFO]   "};

        switch (entry.level) {
            case LogLevel::Info:
                color = ImVec4{0.85f, 0.85f, 0.85f, 1.f};
                prefix = "[INFO]   ";
                break;
            case LogLevel::Warning:
                color = ImVec4{1.f, 0.85f, 0.f, 1.f};
                prefix = "[WARN]   ";
                break;
            case LogLevel::Error:
                color = ImVec4{1.f, 0.3f, 0.3f, 1.f};
                prefix = "[ERROR]  ";
                break;
        }

        ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::TextUnformatted((std::string{prefix} + entry.message).c_str());
        ImGui::PopStyleColor();
    }

    // TODO: understand
    if (_scrollToBottom) {
        ImGui::SetScrollHereY(1.f);
        _scrollToBottom = false;
    }

    ImGui::EndChild();
    ImGui::End();
}
