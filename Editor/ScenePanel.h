#pragma once

#include "IPanel.h"
#include "Platform/Framebuffer.h"
#include "imgui.h"

#include <entt/entt.hpp>
#include <memory>

/**
 * @brief Renders the scene viewport into an ImGui window (centre panel).
 *
 * The panel reads the colour-attachment texture from the provided Framebuffer
 * and blits it as an ImGui::Image.  It also handles viewport resize events so
 * the framebuffer stays in sync with the panel size.
 */
class ScenePanel final : public IPanel {
public:
    explicit ScenePanel(std::shared_ptr<Framebuffer> framebuffer);

    void OnRender(entt::dispatcher& dispatcher, entt::registry& registry) override;

private:
    std::shared_ptr<Framebuffer> _framebuffer{};

    ImVec2 _lastSize{};
    ImVec2 _lastMousePosition{};
};
