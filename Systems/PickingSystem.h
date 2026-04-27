#pragma once

#include "Core/Window.h"
#include "DataStructures/Octree.h"
#include "Events/WindowEvents.h"
#include "Events/ScenePanelEvents.h"

#include "entt/entt.hpp"

#include <optional>

class PickingSystem {
public:
    PickingSystem(Window* window, Octree* octree, entt::dispatcher& dispatcher, entt::registry& registry);

    void Update();

    void DrawPickingRay() const;

    [[nodiscard]] entt::entity GetLastPickedEntity() const { return _lastPickedEntity; }

private:
    void _onMouseButtonPressed(const WindowsEvents::MousePressedEvent& mousePressedEvent);

    void _onScenePanelResized(ScenePanelEvents::ScenePanelResizedEvent scenePanelResizedEvent);

    void _onScenePanelMouseMovedEvent(ScenePanelEvents::ScenePanelMouseMovedEvent scenePanelMouseMovedEvent);

    Window* _window{};

    Octree* _octree{};

    entt::registry& _registry;

    entt::entity _lastPickedEntity{};

    std::optional<std::pair<glm::vec3, glm::vec3>> _lastRay{};

    glm::vec2 _scenePanelMousePosition{};

    glm::vec2 _scenePanelSize{};
};