#include "HierarchyPanel.h"

#include "Components/Mesh.h"
#include "Components/Relationship.h"

#include "imgui.h"

#include <cstdint>

void HierarchyPanel::OnRender(entt::registry& registry) {
    constexpr ImGuiWindowFlags flags{
        ImGuiWindowFlags_NoCollapse};

    ImGui::Begin("Hierarchy", nullptr, flags);

    // ── Header: entity count ──────────────────────────────────────────────
    const auto meshView{registry.view<Mesh>()};
    ImGui::TextDisabled("%zu object(s)", meshView.size());
    ImGui::Separator();

    // ── Entity list ───────────────────────────────────────────────────────
    meshView.each([&](const entt::entity entity, const Mesh& mesh) {
        const bool isSelected{entity == _selectedEntity};

        // Build a display name: "Mesh <id>  [Entity <n>]"
        char label[64];
        std::snprintf(label, sizeof(label),
                      "Mesh %u  [Entity %u]",
                      mesh.meshID,
                      static_cast<unsigned>(entt::to_integral(entity)));

        ImGuiTreeNodeFlags flags =
            ImGuiTreeNodeFlags_OpenOnArrow |
            ImGuiTreeNodeFlags_SpanAvailWidth |
            ImGuiTreeNodeFlags_Leaf;          // no children for now

        if (isSelected) {
            flags |= ImGuiTreeNodeFlags_Selected;
        }

        const bool opened{ImGui::TreeNodeEx(
            reinterpret_cast<void*>(static_cast<intptr_t>(entt::to_integral(entity))),
            flags,
            "%s", label)};

        if (ImGui::IsItemClicked()) {
            _selectedEntity = entity;
        }

        // Right-click context menu
        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::MenuItem("Delete Entity")) {
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
    });

    // Click on empty space → deselect
    if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered()) {
        _selectedEntity = entt::null;
    }

    ImGui::End();
}
