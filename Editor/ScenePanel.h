#pragma once

#include "IPanel.h"
#include "Platform/Framebuffer.h"
#include "imgui.h"

#include <entt/entt.hpp>
#include <memory>

class ScenePanel final : public IPanel {
public:
    explicit ScenePanel(std::shared_ptr<Framebuffer> framebuffer);

    void OnRender(entt::dispatcher& dispatcher, entt::registry& registry) override;

private:
    void _drawGizmo(ImVec2 panelPos, ImVec2 panelSize, entt::registry& registry) const;

    std::shared_ptr<Framebuffer> _framebuffer{};

    ImVec2 _lastSize{};
    ImVec2 _panelPos{};
};
