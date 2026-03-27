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

    void OnRender(entt::registry& registry) override;

    void SetSelectedEntity(entt::entity entity) { _selectedEntity = entity; }

private:
    void _drawTransformComponent(entt::registry& registry) const;

    void _drawMeshComponent(entt::registry& registry) const;

    void _drawPointLightComponent(entt::registry& registry) const;

    void _drawDirectionalLightComponent(entt::registry& registry) const;

    void _drawCameraComponent(entt::registry& registry) const;

    entt::entity _selectedEntity{entt::null};
};
