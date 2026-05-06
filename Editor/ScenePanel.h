#pragma once

#include "Controllers/FramebuffersController.h"
#include "IPanel.h"

#include "imgui.h"

#include <entt/entt.hpp>

class ScenePanel final : public IPanel {
public:
    explicit ScenePanel(const FramebuffersController* framebuffersController);

    void OnRender(entt::dispatcher& dispatcher, entt::registry& registry) override;

private:
    void _drawGizmo(ImVec2 panelPos, ImVec2 panelSize, entt::registry& registry) const;

    const FramebuffersController* _framebuffersController{};

    ImVec2 _lastSize{};
    ImVec2 _panelPos{};
};
