#pragma once

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

    void OnRender(entt::registry& registry) override;

    /** Returns the currently selected entity (entt::null if none). */
    [[nodiscard]] entt::entity GetSelectedEntity() const { return _selectedEntity; }

    /** Allows other panels (e.g. ScenePanel) to clear the selection. */
    void SetSelectedEntity(entt::entity entity) { _selectedEntity = entity; }

private:
    entt::entity _selectedEntity{entt::null};
};
