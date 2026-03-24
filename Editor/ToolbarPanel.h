#pragma once

#include "IPanel.h"
#include "imgui.h"

/**
 * @brief Top toolbar with Play / Pause / Stop buttons.
 *
 * The toolbar is rendered as a full-width ImGui window pinned at the top of
 * the editor layout.  It exposes the current play-state so other systems can
 * query it.
 */
class ToolbarPanel final : public IPanel {
public:
    ToolbarPanel() = default;

    void OnRender(entt::registry& registry) override;

    [[nodiscard]] bool IsPlaying() const { return _isPlaying; }
    [[nodiscard]] bool IsPaused()  const { return _isPaused;  }

private:
    void _renderPlayButton(float buttonWidth);

    void _renderPauseButton(float buttonWidth);

    void _renderStopButton(float buttonWidth);

    bool _isPlaying{};

    bool _isPaused{};

    static constexpr ImVec4 _untoggledColor{.3f, .3f, .3f, 1.f};
};
