#include "HierarchyPanel.h"

#include "Components/Lights/DirectionalLight.h"
#include "Components/Lights/PointLight.h"
#include "Components/MeshNodeTag.h"
#include "Components/Name.h"
#include "Components/Relationship.h"
#include "imgui.h"

void HierarchyPanel::OnRender(entt::registry& registry) {
    constexpr ImGuiWindowFlags hierarchyFlags{ImGuiWindowFlags_NoCollapse};

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

    const bool opened{ImGui::TreeNodeEx(reinterpret_cast<void*>(static_cast<intptr_t>(entt::to_integral(entity))), flags, "%s", displayName)};
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
    const auto meshView{registry.view<MeshNodeTag, Relationship, Name>()};
    //ImGui::TextDisabled("%zu object(s)", meshView.size());
    ImGui::Separator();

    for (const auto entity : meshView) {
        const auto& rel{meshView.get<Relationship>(entity)};

        if (rel.Parent == entt::null) {
            _drawEntityNode(entity, registry);
        }
    }
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

void HierarchyPanel::_drawEntityNode(const entt::entity entity, entt::registry& registry) {
    const auto& name{registry.get<Name>(entity).Value};

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;

    if (ImGui::TreeNodeEx(reinterpret_cast<void*>(entity), flags, "%s", name.c_str())) {
        if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
            _selectedEntity = entity;
        }

        auto view{registry.view<Relationship, MeshNodeTag>()};

        for (auto child : view) {
            const auto& rel = view.get<Relationship>(child);

            if (rel.Parent == entity) {
                _drawEntityNode(child, registry);
            }
        }

        ImGui::TreePop();
    }
}
