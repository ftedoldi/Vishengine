#include "PickingSystem.h"

#include "Components/Camera/Camera.h"
#include "Components/Camera/EditorCameraTag.h"
#include "Components/Name.h"
#include "Components/SelectedTag.h"
#include "Core/Raycaster.h"
#include "RenderingComponents/LineDrawer.h"

#include "GLFW/glfw3.h"
#include "imgui.h"
#include "ImGuizmo.h"
#include "glm/glm.hpp"

#include <iostream>

PickingSystem::PickingSystem(Window* window, Octree* octree, entt::dispatcher& dispatcher, entt::registry& registry)
    : _window{window}, _octree{octree}, _registry{registry} {
    dispatcher.sink<WindowsEvents::MousePressedEvent>().connect<&PickingSystem::_onMouseButtonPressed>(this);
    dispatcher.sink<ScenePanelEvents::ScenePanelResizedEvent>().connect<&PickingSystem::_onScenePanelResized>(this);
    dispatcher.sink<ScenePanelEvents::ScenePanelMouseMovedEvent>().connect<&PickingSystem::_onScenePanelMouseMovedEvent>(this);
}

void PickingSystem::Update() {
}

void PickingSystem::DrawPickingRay() const {
    if (_lastRay) {
        LineDrawer::Get().DrawLine(_lastRay->first, _lastRay->second);
    }
}

void PickingSystem::_onMouseButtonPressed(const WindowsEvents::MousePressedEvent& mousePressedEvent) {
    if (mousePressedEvent.Button != GLFW_MOUSE_BUTTON_LEFT || mousePressedEvent.Action != GLFW_PRESS) {
        return;
    }

    // If im hovering or using a guizmo (that can be placed outside the selected entity) skip the picking otherwise the entity would be deselected.
    if (ImGuizmo::IsOver() || ImGuizmo::IsUsing()) {
        return;
    }

    if (!_octree) {
        return;
    }

    if (_scenePanelSize.x <= 0.f || _scenePanelSize.y <= 0.f) {
        return;
    }

    // Check click is inside the scene viewport.
    const auto mouseX{static_cast<float>(mousePressedEvent.XMousePosition)};
    const auto mouseY{static_cast<float>(mousePressedEvent.YMousePosition)};
    if (mouseX < _scenePanelMousePosition.x || mouseX > _scenePanelMousePosition.x + _scenePanelSize.x ||
        mouseY < _scenePanelMousePosition.y || mouseY > _scenePanelMousePosition.y + _scenePanelSize.y) {
        return;
    }

    // Map click position to NDC relative to the scene panel.
    const float xNDC{(2.f * (mouseX - _scenePanelMousePosition.x)) / _scenePanelSize.x - 1.f};
    const float yNDC{1.f - (2.f * (mouseY - _scenePanelMousePosition.y)) / _scenePanelSize.y};

    const glm::vec4 rayClipSpace{xNDC, yNDC, -1.f, 1.f};

    const auto cameraView{_registry.view<Camera, EditorCameraTag>()};

    for (const auto entity : cameraView) {
        const auto& camera{_registry.get<Camera>(entity)};

        const auto rayEyeSpace{glm::inverse(camera.ProjectionMatrix) * rayClipSpace};
        const glm::vec3 rayEyeDir{rayEyeSpace.x, rayEyeSpace.y, -1.f};

        const Transform cameraWorld{camera.ViewTransform.Invert()};
        const glm::vec3 rayOrigin{cameraWorld.Position};
        const glm::vec3 rayDir{glm::normalize(cameraWorld.Rotation * rayEyeDir)};

        const Ray ray{rayOrigin, rayDir, camera.FarPlaneZDistance};

        _lastRay = {rayOrigin, rayOrigin + camera.FarPlaneZDistance * rayDir};

        if (const auto hit{_octree->Raycast(ray, _registry)}) {
            // Remove the old selected entities
            _removeSelectedEntities();
            _registry.get_or_emplace<SelectedTag>(hit->Entity);
            _drawDebug(*hit);
        } else {
            // If no entity was hit remove all the entities from the selection.
            _removeSelectedEntities();
            std::cout << "No entity picked\n";
        }
    }
}
void PickingSystem::_onScenePanelResized(const ScenePanelEvents::ScenePanelResizedEvent scenePanelResizedEvent) {
    _scenePanelSize = scenePanelResizedEvent.ScenePanelSize;
}

void PickingSystem::_onScenePanelMouseMovedEvent(const ScenePanelEvents::ScenePanelMouseMovedEvent scenePanelMouseMovedEvent) {
    _scenePanelMousePosition = scenePanelMouseMovedEvent.ScenePanelMousePosition;
}

void PickingSystem::_removeSelectedEntities() const {
    const auto selectedEntitiesView{_registry.view<SelectedTag>()};
    for (const auto selectedEntity : selectedEntitiesView) {
        _registry.remove<SelectedTag>(selectedEntity);
    }
}

void PickingSystem::_drawDebug(const RaycastHit& raycastHit) const {
    if (const auto* name{_registry.try_get<Name>(raycastHit.Entity)}) {
        std::cout << "Picked entity: " << name->Value << " (distance: " << raycastHit.Distance << ")\n";
    } else {
        std::cout << "Picked entity id: " << static_cast<std::uint32_t>(raycastHit.Entity) << " (distance: " << raycastHit.Distance << ")\n";
    }
}