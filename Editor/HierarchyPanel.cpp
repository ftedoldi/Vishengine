#include "HierarchyPanel.h"

#include "Components/BoundingBox.h"
#include "Components/Lights/DirectionalLight.h"
#include "Components/Lights/PointLight.h"
#include "Components/MeshNodeTag.h"
#include "Components/Name.h"
#include "Components/Relationship.h"
#include "Components/SelectedTag.h"
#include "Components/Transforms/RelativeTransform.h"
#include "Components/Transforms/TransformDirtyFlag.h"
#include "Components/Transforms/WorldTransform.h"
#include "imgui.h"

void HierarchyPanel::OnRender(entt::dispatcher&, entt::registry& registry) {
    constexpr ImGuiWindowFlags hierarchyFlags{ImGuiWindowFlags_NoCollapse};

    ImGui::Begin("Hierarchy", nullptr, hierarchyFlags);

    _drawMeshes(registry);
    _drawLights(registry);

    // Click on empty space → deselect
    const auto selectedEntitiesView{registry.view<SelectedTag>()};
    if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered()) {
        for (const auto selectedEntity : selectedEntitiesView) {
            registry.remove<SelectedTag>(selectedEntity);
        }
    }

    if (ImGui::BeginPopupContextWindow("hierarchy_context",
            ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
        if (ImGui::MenuItem("Create Empty Entity")) {
            const auto entity{registry.create()};
            registry.emplace<Name>(entity, "Empty Entity");
            registry.emplace<RelativeTransform>(entity);
            registry.emplace<WorldTransform>(entity);
            registry.emplace<TransformDirtyFlag>(entity);
            registry.emplace<Relationship>(entity);
        }
        if (ImGui::BeginMenu("Create Light")) {
            if (ImGui::MenuItem("Point Light")) {
                const auto entity{registry.create()};
                registry.emplace<Name>(entity, "Point Light");
                registry.emplace<PointLight>(entity);
                registry.emplace<BoundingBox>(entity, Box{glm::vec3{-0.15f}, glm::vec3{0.15f}});
                registry.emplace<RelativeTransform>(entity);
                registry.emplace<WorldTransform>(entity);
                registry.emplace<Relationship>(entity);
                registry.emplace<TransformDirtyFlag>(entity);
            }
            if (ImGui::MenuItem("Directional Light")) {
                const auto entity{registry.create()};
                registry.emplace<Name>(entity, "Directional Light");
                registry.emplace<DirectionalLight>(entity);
            }
            ImGui::EndMenu();
        }
        ImGui::EndPopup();
    }

    ImGui::End();
}

void HierarchyPanel::_drawEntity(entt::registry& registry, const entt::entity entity, const char* displayName) {
    const bool isSelected{registry.any_of<SelectedTag>(entity)};

    ImGuiTreeNodeFlags flags =
        ImGuiTreeNodeFlags_OpenOnArrow |
        ImGuiTreeNodeFlags_SpanAvailWidth |
        ImGuiTreeNodeFlags_Leaf;

    if (isSelected) {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    const bool opened{ImGui::TreeNodeEx(reinterpret_cast<void*>(static_cast<intptr_t>(entt::to_integral(entity))), flags, "%s", displayName)};
    if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
        registry.get_or_emplace<SelectedTag>(entity);
    }

    if (ImGui::BeginPopupContextItem("entity_deletion_popup")) {
        if (ImGui::Selectable("Delete entity")) {
            registry.destroy(entity);
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
            registry.get_or_emplace<SelectedTag>(entity);
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
