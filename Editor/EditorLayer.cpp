#include "EditorLayer.h"

#include "imgui.h"

EditorLayer::EditorLayer(std::shared_ptr<Framebuffer> sceneFramebuffer,
                         const std::filesystem::path& assetsRoot)
    : _scene{std::move(sceneFramebuffer)}
    , _contentBrowser{assetsRoot} {}

// ── Public ────────────────────────────────────────────────────────────────────

void EditorLayer::OnRender(entt::registry& registry) {
    _renderMenuBar();
    _layoutPanels(registry);
}

// ── Private ───────────────────────────────────────────────────────────────────

void EditorLayer::_renderMenuBar() {
    if (!ImGui::BeginMainMenuBar()) { return; }

    if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("New Scene",  "Ctrl+N")) { /* TODO */ }
        if (ImGui::MenuItem("Open Scene", "Ctrl+O")) { /* TODO */ }
        ImGui::Separator();
        if (ImGui::MenuItem("Save Scene", "Ctrl+S")) { /* TODO */ }
        ImGui::Separator();
        if (ImGui::MenuItem("Exit", "Alt+F4"))        { /* TODO */ }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Edit")) {
        if (ImGui::MenuItem("Undo", "Ctrl+Z")) { /* TODO */ }
        if (ImGui::MenuItem("Redo", "Ctrl+Y")) { /* TODO */ }
        ImGui::Separator();
        if (ImGui::MenuItem("Preferences"))    { /* TODO */ }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("View")) {
        ImGui::MenuItem("Hierarchy",       nullptr, &_hierarchy.IsVisible);
        ImGui::MenuItem("Inspector",       nullptr, &_inspector.IsVisible);
        ImGui::MenuItem("Scene",           nullptr, &_scene.IsVisible);
        ImGui::MenuItem("Console",         nullptr, &_console.IsVisible);
        ImGui::MenuItem("Content Browser", nullptr, &_contentBrowser.IsVisible);
        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
}

void EditorLayer::_layoutPanels(entt::registry& registry) {
    // ── Compute layout regions ────────────────────────────────────────────
    // We replicate the Unity-like layout from the reference image:
    //
    //  ┌──────────────────────────────────────────────────────────────┐
    //  │  MenuBar  (ImGui main menu bar, handled above)               │
    //  ├──────────────────────────────────────────────────────────────┤
    //  │  Toolbar  (thin strip, centred play/pause/stop)              │
    //  ├────────────┬─────────────────────────────┬───────────────────┤
    //  │ Hierarchy  │        Scene viewport        │    Inspector      │
    //  │            │                              │                   │
    //  ├────────────┴──────────────────────────────┴───────────────────┤
    //  │  Console / Content Browser  (bottom strip)                    │
    //  └──────────────────────────────────────────────────────────────┘

    const ImGuiIO& io{ImGui::GetIO()};
    const float screenW{io.DisplaySize.x};
    const float screenH{io.DisplaySize.y};

    // Reserve space for the main menu bar
    const float menuBarH{ImGui::GetFrameHeight()};

    // Fixed panel widths / heights
    const float toolbarH    {32.f};
    const float hierarchyW  {200.f};
    const float inspectorW  {600.f};
    const float bottomH     {200.f};

    const float topY        {menuBarH};
    const float toolbarY    {topY};
    const float panelsY     {topY + toolbarH};
    const float sceneW      {screenW - hierarchyW - inspectorW};
    const float panelsH     {screenH - menuBarH - toolbarH - bottomH};
    const float bottomY     {panelsY + panelsH};

    // Common flags for fixed, non-movable panels
    constexpr ImGuiWindowFlags panelFlags{
        ImGuiWindowFlags_NoTitleBar   |
        ImGuiWindowFlags_NoResize     |
        ImGuiWindowFlags_NoMove       |
        ImGuiWindowFlags_NoCollapse   };

    // ── Toolbar ───────────────────────────────────────────────────────────
    ImGui::SetNextWindowPos(ImVec2{0.f, toolbarY});
    ImGui::SetNextWindowSize(ImVec2{screenW, toolbarH});
    _toolbar.OnRender(registry);   // ToolbarPanel uses its own Begin/End

    // ── Hierarchy ─────────────────────────────────────────────────────────
    if (_hierarchy.IsVisible) {
        ImGui::SetNextWindowPos(ImVec2{0.f, panelsY});
        ImGui::SetNextWindowSize(ImVec2{hierarchyW, panelsH});
        ImGui::SetNextWindowBgAlpha(1.f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.f);
        // Override Begin inside HierarchyPanel — we set pos/size before it calls Begin
        _hierarchy.OnRender(registry);
        ImGui::PopStyleVar(2);
    }

    // ── Scene viewport ────────────────────────────────────────────────────
    if (_scene.IsVisible) {
        ImGui::SetNextWindowPos(ImVec2{hierarchyW, panelsY});
        ImGui::SetNextWindowSize(ImVec2{sceneW, panelsH});
        ImGui::SetNextWindowBgAlpha(1.f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.f);
        _scene.OnRender(registry);
        ImGui::PopStyleVar(2);
    }

    // ── Inspector ─────────────────────────────────────────────────────────
    if (_inspector.IsVisible) {
        _inspector.SetSelectedEntity(_hierarchy.GetSelectedEntity());
        ImGui::SetNextWindowPos(ImVec2{hierarchyW + sceneW, panelsY});
        ImGui::SetNextWindowSize(ImVec2{inspectorW, panelsH});
        ImGui::SetNextWindowBgAlpha(1.f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.f);
        _inspector.OnRender(registry);
        ImGui::PopStyleVar(2);
    }

    // ── Bottom strip: Console (left) + Content Browser (right) ───────────
    const float consolePanelW{screenW * 0.35f};
    const float contentBrowserW{screenW - consolePanelW};

    if (_console.IsVisible) {
        ImGui::SetNextWindowPos(ImVec2{0.f, bottomY});
        ImGui::SetNextWindowSize(ImVec2{consolePanelW, bottomH});
        ImGui::SetNextWindowBgAlpha(1.f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.f);
        _console.OnRender(registry);
        ImGui::PopStyleVar(2);
    }

    if (_contentBrowser.IsVisible) {
        ImGui::SetNextWindowPos(ImVec2{consolePanelW, bottomY});
        ImGui::SetNextWindowSize(ImVec2{contentBrowserW, bottomH});
        ImGui::SetNextWindowBgAlpha(1.f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.f);
        _contentBrowser.OnRender(registry);
        ImGui::PopStyleVar(2);
    }
}
