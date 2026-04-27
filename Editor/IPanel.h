#pragma once

#include <entt/entt.hpp>

/**
 * @brief Base interface for all editor panels.
 *
 * Every panel must implement OnRender() which is called once per frame
 * inside the ImGui render loop.
 */
class IPanel {
public:
    virtual ~IPanel() = default;

    /**
     * @brief Called every frame to draw the panel.
     * @param dispatcher The event dispatcher.
     * @param registry  The ECS registry (read/write access).
     */
    virtual void OnRender(entt::dispatcher& dispatcher, entt::registry& registry) = 0;

    /** @brief Whether the panel is currently visible. */
    bool IsVisible{true};
};
