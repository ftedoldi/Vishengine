#pragma once

#include "DataStructures/Octree.h"
#include "Events/WindowEvents.h"
#include "Events/ScenePanelEvents.h"

#include "entt/entt.hpp"

#include <optional>

class PickingSystem {
public:
    PickingSystem(Octree* octree, entt::dispatcher& dispatcher, entt::registry& registry);

    void Update();

    void DrawPickingRay() const;

private:
    void _onMouseButtonPressed(const WindowsEvents::MousePressedEvent& mousePressedEvent);

    void _onScenePanelResized(ScenePanelEvents::ScenePanelResizedEvent scenePanelResizedEvent);

    void _onScenePanelMouseMovedEvent(ScenePanelEvents::ScenePanelMouseMovedEvent scenePanelMouseMovedEvent);

    void _removeSelectedEntities() const;

    void _drawDebug(const RaycastHit& raycastHit) const;

    Octree* _octree{};

    entt::registry& _registry;

    std::optional<std::pair<glm::vec3, glm::vec3>> _lastRay{};

    glm::vec2 _scenePanelMousePosition{};

    glm::vec2 _scenePanelSize{};
};