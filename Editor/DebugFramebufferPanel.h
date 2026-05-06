#pragma once

#include "Controllers/FramebuffersController.h"
#include "IPanel.h"

#include <entt/entt.hpp>

class DebugFramebufferPanel final : public IPanel {
public:
    explicit DebugFramebufferPanel(const FramebuffersController* framebuffersController);

    void OnRender(entt::dispatcher& dispatcher, entt::registry& registry) override;

private:
    const FramebuffersController* _framebuffersController{};

    entt::entity _debugViewEntity{};

    bool _wasVisible{};
};