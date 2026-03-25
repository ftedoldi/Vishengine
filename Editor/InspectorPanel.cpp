#include "InspectorPanel.h"

#include "Components/Mesh.h"
#include "Components/Position.h"
#include "Components/Rotation.h"
#include "Components/Scale.h"
#include "Components/Lights/PointLight.h"
#include "Components/Lights/DirectionalLight.h"

#include "Components/WorldTransform.h"
#include "imgui.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace {

    void DrawVec3Control(const char* label, glm::vec3& values) {
    ImGui::PushID(label);

    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, 100.f);
    ImGui::Text("%s", label);
    ImGui::NextColumn();

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});

    const float lineHeight{ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.f};
    const ImVec2 buttonSize{lineHeight + 3.f, lineHeight};
    const float inputWidth{(ImGui::GetContentRegionAvail().x - buttonSize.x * 3.f) / 3.f};

    // X
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{1.f, 0.f, 0.f, 1.f});
    ImGui::BeginDisabled(true);
    ImGui::Button("X", buttonSize);
    ImGui::EndDisabled();
    ImGui::PopStyleColor(1);

    ImGui::SameLine();

    ImGui::SetNextItemWidth(inputWidth);
    ImGuiTextFilter xTextFilter{};
    char xPosition[5];
    std::snprintf(xPosition, sizeof(xPosition), "%.10f", values.x); // 2 decimal places
    if (ImGui::InputTextWithHint("##XTextFilter", xPosition, xTextFilter.InputBuf, IM_ARRAYSIZE(xTextFilter.InputBuf), ImGuiInputTextFlags_EscapeClearsAll | ImGuiInputTextFlags_EnterReturnsTrue)) {
        xTextFilter.Build();
        values.x = std::stof(xTextFilter.InputBuf);
    }
    ImGui::SameLine();

    // Y
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.f, 1.f, 0.f, 1.f});
    ImGui::BeginDisabled(true);
    ImGui::Button("Y", buttonSize);
    ImGui::EndDisabled();
    ImGui::PopStyleColor(1);

    ImGui::SameLine();

    ImGui::SetNextItemWidth(inputWidth);
    ImGuiTextFilter yTextFilter{};
    char yPosition[5];
    std::snprintf(yPosition, sizeof(yPosition), "%.10f", values.y); // 2 decimal places
    if (ImGui::InputTextWithHint("##YTextFilter", yPosition, yTextFilter.InputBuf, IM_ARRAYSIZE(yTextFilter.InputBuf), ImGuiInputTextFlags_EscapeClearsAll | ImGuiInputTextFlags_EnterReturnsTrue)) {
        yTextFilter.Build();
        values.y = std::stof(yTextFilter.InputBuf);
    }
    ImGui::SameLine();

    // Z
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.f, 0.f, 1.f, 1.f});
    ImGui::BeginDisabled(true);
    ImGui::Button("Z", buttonSize);
    ImGui::EndDisabled();
    ImGui::PopStyleColor(1);

    ImGui::SameLine();

    ImGui::SetNextItemWidth(inputWidth);
    ImGuiTextFilter zTextFilter{};
    char zPosition[5];
    std::snprintf(zPosition, sizeof(zPosition), "%.10f", values.z); // 2 decimal places
    if (ImGui::InputTextWithHint("##ZTextFilter", zPosition, zTextFilter.InputBuf, IM_ARRAYSIZE(zTextFilter.InputBuf), ImGuiInputTextFlags_EscapeClearsAll | ImGuiInputTextFlags_EnterReturnsTrue)) {
        zTextFilter.Build();
        values.z = std::stof(zTextFilter.InputBuf);
    }

    ImGui::PopStyleVar();
    ImGui::Columns(1);
    ImGui::PopID();
}

}

void InspectorPanel::OnRender(entt::registry& registry) {
    constexpr ImGuiWindowFlags flags{ImGuiWindowFlags_None};

    ImGui::Begin("Inspector", nullptr, flags);

    if (_selectedEntity == entt::null || !registry.valid(_selectedEntity)) {
        ImGui::TextDisabled("No entity selected.");
        ImGui::End();
        return;
    }

    // Entity header
    ImGui::Text("Entity  %u", static_cast<unsigned>(entt::to_integral(_selectedEntity)));
    ImGui::Separator();

    _drawMeshComponent(registry);
    _drawPointLightComponent(registry);
    _drawDirectionalLightComponent(registry);

    ImGui::Spacing();
    ImGui::Separator();

    // Add Component button
    const float buttonWidth{ImGui::GetContentRegionAvail().x};
    if (ImGui::Button("Add Component", ImVec2{buttonWidth, 0.f})) {
        ImGui::OpenPopup("AddComponentPopup");
    }

    if (ImGui::BeginPopup("AddComponentPopup")) {
        if (ImGui::MenuItem("Position") && !registry.all_of<Position>(_selectedEntity)) {
            registry.emplace<Position>(_selectedEntity);
        }
        if (ImGui::MenuItem("Rotation") && !registry.all_of<Rotation>(_selectedEntity)) {
            registry.emplace<Rotation>(_selectedEntity);
        }
        if (ImGui::MenuItem("Scale") && !registry.all_of<Scale>(_selectedEntity)) {
            registry.emplace<Scale>(_selectedEntity);
        }
        ImGui::EndPopup();
    }

    ImGui::End();
}

void InspectorPanel::_drawTransformComponent(entt::registry& registry) const {
    if (!registry.all_of<Position>(_selectedEntity) &&
        !registry.all_of<Rotation>(_selectedEntity) &&
        !registry.all_of<Scale>(_selectedEntity)) {
        return;
    }

    const ImGuiTreeNodeFlags flags{
        ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
        ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowOverlap |
        ImGuiTreeNodeFlags_FramePadding};

    if (ImGui::TreeNodeEx("##Transform", flags, "Transform")) {
        // Position
        if (registry.all_of<Position>(_selectedEntity)) {
            auto& pos{registry.get<Position>(_selectedEntity)};
            DrawVec3Control("Position", pos.Vector);
        }

        // Rotation (stored as quaternion, edited as Euler degrees)
        if (registry.all_of<Rotation>(_selectedEntity)) {
            auto& rot{registry.get<Rotation>(_selectedEntity)};
            glm::vec3 euler{glm::degrees(glm::eulerAngles(rot.Quaternion))};
            if (DrawVec3Control("Rotation", euler), true) {
                rot.Quaternion = glm::quat{glm::radians(euler)};
            }
        }

        // Scale (uniform float)
        if (registry.all_of<Scale>(_selectedEntity)) {
            auto& scale{registry.get<Scale>(_selectedEntity)};
            ImGui::Columns(2);
            ImGui::SetColumnWidth(0, 100.f);
            ImGui::Text("Scale");
            ImGui::NextColumn();
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            ImGui::DragFloat("##Scale", &scale.Value, 0.01f, 0.001f, 100.f, "%.3f");
            ImGui::Columns(1);
        }

        ImGui::TreePop();
    }
}

void InspectorPanel::_drawMeshComponent(entt::registry& registry) const {
    if (!registry.all_of<Mesh>(_selectedEntity)) {
        return;
    }

    const ImGuiTreeNodeFlags flags{
        ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
        ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding};

    if (ImGui::TreeNodeEx("##Mesh", flags, "Mesh")) {
        const auto& mesh{registry.get<Mesh>(_selectedEntity)};
        ImGui::Text("Mesh ID : %u", mesh.meshID);
        ImGui::TreePop();
    }

    _drawTransformComponent(registry);
}

void InspectorPanel::_drawPointLightComponent(entt::registry& registry) const {
    if (!registry.all_of<PointLight>(_selectedEntity)) {
        return;
    }
    const ImGuiTreeNodeFlags flags{
        ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
        ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding};

    if (ImGui::TreeNodeEx("##PointLight", flags, "Point Light")) {
        auto& pointLight{registry.get<PointLight>(_selectedEntity)};
        ImGui::ColorEdit3("Ambient",  &pointLight.Ambient.x);
        ImGui::ColorEdit3("Diffuse",  &pointLight.Diffuse.x);
        ImGui::ColorEdit3("Specular", &pointLight.Specular.x);
        ImGui::TreePop();
    }

    _drawTransformComponent(registry);
}

void InspectorPanel::_drawDirectionalLightComponent(entt::registry& registry) const {
    if (!registry.all_of<DirectionalLight>(_selectedEntity)) {
        return;
    }
    const ImGuiTreeNodeFlags flags{
        ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
        ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding};

    if (ImGui::TreeNodeEx("##DirectionalLight", flags, "Directional Light")) {
        auto& directionalLight{registry.get<DirectionalLight>(_selectedEntity)};
        DrawVec3Control("Direction", directionalLight.Direction);
        ImGui::ColorEdit3("Ambient",  &directionalLight.Ambient.x);
        ImGui::ColorEdit3("Diffuse",  &directionalLight.Diffuse.x);
        ImGui::ColorEdit3("Specular", &directionalLight.Specular.x);
        ImGui::TreePop();
    }
}

