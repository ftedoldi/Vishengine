#include "HierarchyPanel.h"

#include "Components/Mesh.h"

#include "Components/Lights/DirectionalLight.h"
#include "Components/Lights/PointLight.h"
#include "Components/Relationship.h"
#include "imgui.h"

void HierarchyPanel::OnRender(entt::registry& registry) {
    constexpr ImGuiWindowFlags hierarchyFlags{
        ImGuiWindowFlags_NoCollapse};

    ImGui::Begin("Hierarchy", nullptr, hierarchyFlags);

    _drawMeshes(registry);
    _drawLights(registry);

    // Click on empty space → deselect
    if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered()) {
        _selectedEntity = entt::null;
    }

    if (ImGui::BeginPopupContextWindow("entity_creation_popup", ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight)) {
        if (ImGui::Selectable("Create entity")) {
            _selectedEntity = registry.create();
        }
        ImGui::EndPopup();
    }

    ImGui::End();
}

void HierarchyPanel::_drawEntity(entt::registry& registry, const entt::entity entity, const char* displayName) {
    const bool isSelected{entity == _selectedEntity};

    ImGuiTreeNodeFlags flags =
        ImGuiTreeNodeFlags_OpenOnArrow |
        ImGuiTreeNodeFlags_SpanAvailWidth |
        ImGuiTreeNodeFlags_Leaf;

    if (isSelected) {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    const bool opened{ImGui::TreeNodeEx(
        reinterpret_cast<void*>(static_cast<intptr_t>(entt::to_integral(entity))),
        flags,
        "%s", displayName)};

    if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
        _selectedEntity = entity;
    }

    if (ImGui::BeginPopupContextItem("entity_deletion_popup")) {
        if (ImGui::Selectable("Delete entity")) {
            registry.destroy(entity);
            if (_selectedEntity == entity) {
                _selectedEntity = entt::null;
            }
        }
        ImGui::EndPopup();
    }

    if (opened) {
        ImGui::TreePop();
    }
}

void HierarchyPanel::_drawMeshes(entt::registry& registry) {
    const auto meshView{registry.view<Mesh>()};
    ImGui::TextDisabled("%zu object(s)", meshView.size());
    ImGui::Separator();

    auto view = registry.view<Relationship>();

    for (auto entity : view) {
        const auto& rel = view.get<Relationship>(entity);

        if (rel.parent == entt::null) {
            _drawEntityNode(entity, registry);
        }
    }

    // Entity list
    /*meshView.each([&registry, this](const entt::entity entity, const Mesh mesh) {
        // Build a display name: "Mesh <id>  [Entity <n>]"
        char label[64];
        std::snprintf(label, sizeof(label),
                      "Mesh %u  [Entity %u]",
                      mesh.meshID,
                      static_cast<unsigned>(entt::to_integral(entity)));

        _drawEntity(registry, entity, label);
    });*/
}

void HierarchyPanel::_drawLights(entt::registry& registry) {
    const auto pointLightView{registry.view<PointLight>()};
    ImGui::TextDisabled("%zu object(s)", pointLightView.size());
    ImGui::Separator();

    // Entity list
    pointLightView.each([&registry, this](const entt::entity entity, const PointLight&) {
        char label[64];
        std::snprintf(label, sizeof(label), "PointLight");

        _drawEntity(registry, entity, label);
    });

    const auto directionalLightView{registry.view<DirectionalLight>()};
    ImGui::TextDisabled("%zu object(s)", directionalLightView.size());
    ImGui::Separator();

    // Entity list
    directionalLightView.each([&registry, this](const entt::entity entity, const DirectionalLight&) {
        char label[64];
        std::snprintf(label, sizeof(label), "DirectionalLight");

        _drawEntity(registry, entity, label);
    });
}

void HierarchyPanel::_drawEntityNode(entt::entity entity, entt::registry& registry) {
    const char* name = "Entity"; // or your Name component

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;

    if (ImGui::TreeNodeEx(reinterpret_cast<void*>(entity), flags, "%s", name)) {
        // Draw children

        if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
            _selectedEntity = entity;
        }

        auto view = registry.view<Relationship>();

        for (auto child : view) {
            const auto& rel = view.get<Relationship>(child);

            if (rel.parent == entity) {
                _drawEntityNode(child, registry);
            }
        }

        ImGui::TreePop();
    }
}
