#pragma once

#include "ContentBrowserPanel.h"
#include "ConsolePanel.h"
#include "HierarchyPanel.h"
#include "InspectorPanel.h"
#include "ScenePanel.h"
#include "ToolbarPanel.h"
#include "Platform/Framebuffer.h"

#include "imgui.h"

#include <entt/entt.hpp>
#include <memory>

class EditorLayer {
public:
    explicit EditorLayer(std::shared_ptr<Framebuffer> sceneFramebuffer,
                         const std::filesystem::path& assetsRoot);

    void OnRender(entt::dispatcher& dispatcher, entt::registry& registry);

    [[nodiscard]] bool IsPlaying() const;

    [[nodiscard]] bool IsPaused()  const;

private:
    void _renderMenuBar();

    void _layoutPanels(entt::dispatcher& dispatcher, entt::registry& registry);

    ToolbarPanel        _toolbar{};
    HierarchyPanel      _hierarchy{};
    InspectorPanel      _inspector{};
    ScenePanel          _scene;
    ConsolePanel        _console{};
    ContentBrowserPanel _contentBrowser;
};