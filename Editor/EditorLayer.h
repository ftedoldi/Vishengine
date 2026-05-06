#pragma once

#include "ConsolePanel.h"
#include "ContentBrowserPanel.h"
#include "Controllers/FramebuffersController.h"
#include "HierarchyPanel.h"
#include "InspectorPanel.h"
#include "ScenePanel.h"
#include "ToolbarPanel.h"

#include <entt/entt.hpp>
#include <filesystem>

class EditorLayer {
public:
    explicit EditorLayer(const FramebuffersController* framebuffersController,
                         const std::filesystem::path& assetsRoot);

    void OnRender(entt::dispatcher& dispatcher, entt::registry& registry);

    [[nodiscard]] bool IsPlaying() const;

    [[nodiscard]] bool IsPaused()  const;

private:
    void _renderMenuBar();

    void _layoutPanels(entt::dispatcher& dispatcher, entt::registry& registry);

    ToolbarPanel _toolbar{};
    HierarchyPanel _hierarchy{};
    InspectorPanel _inspector{};
    ScenePanel _scene;
    ConsolePanel _console{};
    ContentBrowserPanel _contentBrowser;
};