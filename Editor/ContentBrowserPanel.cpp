#include "ContentBrowserPanel.h"

#include "imgui.h"

#include <algorithm>
#include <string>

ContentBrowserPanel::ContentBrowserPanel(const std::filesystem::path& assetsRoot)
    : _assetsRoot{assetsRoot}
    , _currentDir{assetsRoot} {}

void ContentBrowserPanel::OnRender(entt::registry& /*registry*/) {
    constexpr ImGuiWindowFlags flags{
        ImGuiWindowFlags_NoCollapse};

    ImGui::Begin("Content Browser", nullptr, flags);

    // ── Left pane: directory tree ─────────────────────────────────────────
    ImGui::BeginChild("##DirTree",
                      ImVec2{180.f, 0.f},
                      true);

    ImGui::TextDisabled("Assets");
    ImGui::Separator();

    if (std::filesystem::exists(_assetsRoot)) {
        _renderDirectoryTree(_assetsRoot);
    } else {
        ImGui::TextDisabled("(Assets folder not found)");
    }

    ImGui::EndChild();

    ImGui::SameLine();

    // ── Right pane: contents ──────────────────────────────────────────────
    ImGui::BeginChild("##Contents", ImVec2{0.f, 0.f}, false);

    _renderBreadcrumb();
    ImGui::Separator();
    _renderContents();

    ImGui::EndChild();

    ImGui::End();
}

void ContentBrowserPanel::_renderBreadcrumb() {
    // Build path relative to assets root and show clickable segments
    std::filesystem::path rel{std::filesystem::relative(_currentDir, _assetsRoot.parent_path())};

    std::filesystem::path accumulated{_assetsRoot.parent_path()};
    bool first{true};

    for (const auto& part : rel) {
        if (!first) { ImGui::SameLine(); ImGui::TextDisabled("/"); ImGui::SameLine(); }
        first = false;
        accumulated /= part;

        const std::filesystem::path captured{accumulated};
        if (ImGui::SmallButton(part.string().c_str())) {
            if (std::filesystem::is_directory(captured)) {
                _currentDir = captured;
            }
        }
    }
}

void ContentBrowserPanel::_renderDirectoryTree(const std::filesystem::path& dir) {
    if (!std::filesystem::is_directory(dir)) { return; }

    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        if (!entry.is_directory()) { continue; }

        const std::string name{entry.path().filename().string()};
        const bool isSelected{entry.path() == _currentDir};

        ImGuiTreeNodeFlags flags{ImGuiTreeNodeFlags_OpenOnArrow |
                                  ImGuiTreeNodeFlags_SpanAvailWidth};
        if (isSelected) { flags |= ImGuiTreeNodeFlags_Selected; }

        const bool opened{ImGui::TreeNodeEx(name.c_str(), flags, "%s", name.c_str())};

        if (ImGui::IsItemClicked()) {
            _currentDir = entry.path();
        }

        if (opened) {
            _renderDirectoryTree(entry.path());
            ImGui::TreePop();
        }
    }
}

void ContentBrowserPanel::_renderContents() {
    if (!std::filesystem::exists(_currentDir)) {
        ImGui::TextDisabled("Directory not found.");
        return;
    }

    // Navigate up button
    if (_currentDir != _assetsRoot) {
        if (ImGui::Button("..")) {
            _currentDir = _currentDir.parent_path();
        }
        ImGui::SameLine();
    }

    const float cellSize{ThumbnailSize + Padding};
    const float panelWidth{ImGui::GetContentRegionAvail().x};
    int columnCount{static_cast<int>(panelWidth / cellSize)};
    if (columnCount < 1) {
        columnCount = 1;
    }

    ImGui::Columns(columnCount, nullptr, false);

    // Collect and sort entries: directories first, then files
    std::vector<std::filesystem::directory_entry> entries;
    for (const auto& entry : std::filesystem::directory_iterator(_currentDir)) {
        entries.push_back(entry);
    }
    std::ranges::sort(entries,
              [](const auto& a, const auto& b) {
                  if (a.is_directory() != b.is_directory()) {
                      return a.is_directory() > b.is_directory();
                  }
                  return a.path().filename() < b.path().filename();
              });

    for (const auto& entry : entries) {
        const std::string filename{entry.path().filename().string()};
        const bool isDir{entry.is_directory()};

        // Icon colour: orange for folders, grey for files
        const ImVec4 iconColour{isDir
            ? ImVec4{0.95f, 0.55f, 0.1f, 1.f}
            : ImVec4{0.7f,  0.7f,  0.7f, 1.f}};

        ImGui::PushID(filename.c_str());

        // Draw a coloured button as a stand-in icon
        ImGui::PushStyleColor(ImGuiCol_Button,        iconColour);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{iconColour.x + 0.1f,
                                                              iconColour.y + 0.1f,
                                                              iconColour.z + 0.1f, 1.f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  iconColour);

        ImGui::Button(isDir ? "  [D]  " : "  [F]  ",
                      ImVec2{ThumbnailSize, ThumbnailSize});

        ImGui::PopStyleColor(3);

        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
            if (isDir) {
                _currentDir = entry.path();
            }
        }

        // Truncate long names
        std::string displayName{filename};
        if (displayName.size() > 12) {
            displayName = displayName.substr(0, 10) + "..";
        }
        ImGui::TextWrapped("%s", displayName.c_str());

        ImGui::NextColumn();
        ImGui::PopID();
    }

    ImGui::Columns(1);
}
