
#pragma once

#include "ContentBrowserPanel.h"
#include "ConsolePanel.h"
#include "HierarchyPanel.h"
#include "InspectorPanel.h"
#include "ScenePanel.h"
#include "ToolbarPanel.h"
#include "Platform/Framebuffer.h"

#include <entt/entt.hpp>
#include <memory>

/**
 * @brief Orchestrates all editor panels and the fixed-layout UI.
 *
 * EditorLayer owns every panel instance and is responsible for:
 *  - Computing and applying the fixed panel layout each frame.
 *  - Rendering the main menu bar (File / Edit / View).
 *  - Forwarding the selected entity from HierarchyPanel to InspectorPanel.
 *  - Calling OnRender() on every panel.
 */
class EditorLayer {
public:
    /**
     * @param sceneFramebuffer  The framebuffer whose colour attachment is
     *                          displayed in the ScenePanel viewport.
     * @param assetsRoot        Absolute path to the project Assets/ directory.
     */
    explicit EditorLayer(std::shared_ptr<Framebuffer> sceneFramebuffer,
                         const std::filesystem::path& assetsRoot);

    /** Called once per frame between ImGui::NewFrame() and ImGui::Render(). */
    void OnRender(entt::registry& registry);

    /** Convenience accessors used by Game to query editor state. */
    [[nodiscard]] bool IsPlaying() const { return _toolbar.IsPlaying(); }
    [[nodiscard]] bool IsPaused()  const { return _toolbar.IsPaused();  }

    ConsolePanel& GetConsole() { return _console; }

private:
    void _renderMenuBar();
    void _layoutPanels(entt::registry& registry);

    ToolbarPanel        _toolbar{};
    HierarchyPanel      _hierarchy{};
    InspectorPanel      _inspector{};
    ScenePanel          _scene;
    ConsolePanel        _console{};
    ContentBrowserPanel _contentBrowser;

    // ── Persistent layout sizes (user-resizable via splitters) ────────────
    float _hierarchyW  {200.f};   ///< Width of the Hierarchy panel
    float _inspectorW  {260.f};   ///< Width of the Inspector panel
    float _bottomH     {200.f};   ///< Height of the bottom strip (Console + Content Browser)
    float _consoleFrac {0.35f};   ///< Fraction of the bottom strip width used by the Console
};

