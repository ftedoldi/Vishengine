#pragma once

#include "IPanel.h"

#include <entt/entt.hpp>

/**
 * @brief Displays and edits the components of the selected entity (right panel).
 *
 * Shows Transform (Position, Rotation, Scale) and Mesh info.
 * The selected entity is provided by HierarchyPanel via SetSelectedEntity().
 */
class InspectorPanel final : public IPanel {
public:
    InspectorPanel() = default;

    void OnRender(entt::dispatcher& dispatcher, entt::registry& registry) override;

private:
    void _drawTransformComponent(entt::entity selectedEntity, entt::registry& registry) const;

    void _drawMeshComponent(entt::entity selectedEntity, entt::registry& registry) const;

    void _drawPointLightComponent(entt::entity selectedEntity, entt::registry& registry) const;

    void _drawDirectionalLightComponent(entt::entity selectedEntity, entt::registry& registry) const;

    void _drawCameraComponent(entt::entity selectedEntity, entt::registry& registry) const;

    void _drawAddComponentMenu(entt::entity selectedEntity, entt::registry& registry) const;
};
