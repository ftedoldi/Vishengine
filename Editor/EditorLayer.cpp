#include "EditorLayer.h"

#include "imgui.h"
#include "imgui_internal.h"

namespace {

void BuildInitialDockLayout(const ImGuiID dockId, const ImVec2 size) {
    ImGui::DockBuilderRemoveNode(dockId);
    ImGui::DockBuilderAddNode(dockId, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(dockId, size);

    // Top: toolbar strip (~5% of height).
    ImGuiID dockToolbar{};
    ImGuiID dockBody{};
    ImGui::DockBuilderSplitNode(dockId, ImGuiDir_Up, 0.05f, &dockToolbar, &dockBody);

    // Bottom: console + content browser (~25% of remaining height).
    ImGuiID dockBottom{};
    ImGuiID dockMiddle{};
    ImGui::DockBuilderSplitNode(dockBody, ImGuiDir_Down, 0.25f, &dockBottom, &dockMiddle);

    ImGuiID dockConsole{};
    ImGuiID dockContent{};
    ImGui::DockBuilderSplitNode(dockBottom, ImGuiDir_Left, 0.4f, &dockConsole, &dockContent);

    // Middle row: hierarchy | scene | inspector.
    ImGuiID dockHierarchy{};
    ImGuiID dockCenterRight{};
    ImGui::DockBuilderSplitNode(dockMiddle, ImGuiDir_Left, 0.15f, &dockHierarchy, &dockCenterRight);

    ImGuiID dockInspector{};
    ImGuiID dockScene{};
    ImGui::DockBuilderSplitNode(dockCenterRight, ImGuiDir_Right, 0.28f, &dockInspector, &dockScene);

    ImGui::DockBuilderDockWindow("##Toolbar", dockToolbar);
    ImGui::DockBuilderDockWindow("Hierarchy", dockHierarchy);
    ImGui::DockBuilderDockWindow("Scene",dockScene);
    ImGui::DockBuilderDockWindow("Inspector", dockInspector);
    ImGui::DockBuilderDockWindow("Console",dockConsole);
    ImGui::DockBuilderDockWindow("Content Browser", dockContent);

    // Hide the tab bar on the toolbar strip so it looks like a proper toolbar.
    if (auto* toolbarNode{ImGui::DockBuilderGetNode(dockToolbar)}) {
        toolbarNode->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
    }

    ImGui::DockBuilderFinish(dockId);
}

}

EditorLayer::EditorLayer(std::shared_ptr<Framebuffer> sceneFramebuffer,
                         const std::filesystem::path& assetsRoot)
    : _scene{std::move(sceneFramebuffer)}
    , _contentBrowser{assetsRoot} {}

bool EditorLayer::IsPlaying() const {
    return _toolbar.IsPlaying();
}

bool EditorLayer::IsPaused() const {
    return _toolbar.IsPaused();
}

void EditorLayer::OnRender(entt::dispatcher& dispatcher, entt::registry& registry) {
    _renderMenuBar();
    _layoutPanels(dispatcher, registry);
}

void EditorLayer::_renderMenuBar() {
    if (!ImGui::BeginMainMenuBar()) { return; }

    if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("New Scene",  "Ctrl+N")) {}
        if (ImGui::MenuItem("Open Scene", "Ctrl+O")) {}
        ImGui::Separator();
        if (ImGui::MenuItem("Save Scene", "Ctrl+S")) {}
        ImGui::Separator();
        if (ImGui::MenuItem("Exit", "Alt+F4"))       {}
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Edit")) {
        if (ImGui::MenuItem("Undo", "Ctrl+Z")) {}
        if (ImGui::MenuItem("Redo", "Ctrl+Y")) {}
        ImGui::Separator();
        if (ImGui::MenuItem("Preferences"))    {}
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

void EditorLayer::_layoutPanels(entt::dispatcher& dispatcher, entt::registry& registry) {
    // Fullscreen transparent host window that owns the dockspace.
    const ImGuiViewport* const vp{ImGui::GetMainViewport()};
    ImGui::SetNextWindowPos(vp->WorkPos);
    ImGui::SetNextWindowSize(vp->WorkSize);
    ImGui::SetNextWindowViewport(vp->ID);

    constexpr ImGuiWindowFlags kHostFlags{
        ImGuiWindowFlags_NoTitleBar            |
        ImGuiWindowFlags_NoCollapse            |
        ImGuiWindowFlags_NoResize              |
        ImGuiWindowFlags_NoMove                |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus            |
        ImGuiWindowFlags_NoBackground          };

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,   0.f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,    ImVec2{0.f, 0.f});
    ImGui::Begin("##DockspaceHost", nullptr, kHostFlags);
    ImGui::PopStyleVar(3);

    const ImGuiID dockId{ImGui::GetID("MainDockspace")};
    ImGui::DockSpace(dockId, ImVec2{0.f, 0.f}, ImGuiDockNodeFlags_None);

    // Build the default layout once. Skipped when imgui.ini already has a saved layout.
    if (const auto* node{ImGui::DockBuilderGetNode(dockId)}; !node || node->IsLeafNode()) {
        BuildInitialDockLayout(dockId, vp->WorkSize);
    }

    ImGui::End();

    if (_toolbar.IsVisible) {
        _toolbar.OnRender(dispatcher, registry);
    }

    if (_hierarchy.IsVisible) {
        _hierarchy.OnRender(dispatcher, registry);
    }

    if (_scene.IsVisible) {
        _scene.OnRender(dispatcher, registry);
    }

    if (_inspector.IsVisible) {
        _inspector.OnRender(dispatcher, registry);
    }

    if (_console.IsVisible) {
        _console.OnRender(dispatcher, registry);
    }

    if (_contentBrowser.IsVisible) {
        _contentBrowser.OnRender(dispatcher, registry);
    }
}
