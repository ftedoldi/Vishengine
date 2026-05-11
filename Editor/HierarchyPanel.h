#pragma once

#include "Controllers/MeshController.h"
#include "IPanel.h"

#include <entt/entt.hpp>

/**
 * @brief Displays the scene entity hierarchy (left panel).
 *
 * Shows every entity that has at least a Mesh component.
 * Clicking an entity selects it and the InspectorPanel will show its details.
 */
class HierarchyPanel final : public IPanel {
public:
    HierarchyPanel() = default;

    void OnRender(entt::dispatcher& dispatcher, entt::registry& registry) override;

private:
    void _drawEntity(entt::dispatcher& dispatcher, entt::registry& registry, entt::entity entity, const char* displayName);

    void _drawMeshes(entt::dispatcher& dispatcher, entt::registry& registry);

    void _drawLights(entt::dispatcher& dispatcher, entt::registry& registry);

    void _drawEntityNode(entt::dispatcher& dispatcher, entt::entity entity, entt::registry& registry);
};
