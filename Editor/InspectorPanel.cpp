#include "InspectorPanel.h"

#include "Components/Mesh.h"
#include "Components/Lights/PointLight.h"
#include "Components/Lights/DirectionalLight.h"

#include "Components/Camera/Camera.h"
#include "Components/MeshNodeTag.h"
#include "Components/Name.h"
#include "Components/Transforms/RelativeTransform.h"
#include "Components/Transforms/TransformDirtyFlag.h"
#include "imgui.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <optional>

namespace {
// Return whether a position was modified.
bool DrawAndUpdateVec3Widget(const char* label, glm::vec3& values) {
    bool isValueModified{};
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
    std::snprintf(xPosition, sizeof(xPosition), "%.10f", values.x);
    if (ImGui::InputTextWithHint("##XTextFilter", xPosition, xTextFilter.InputBuf, IM_ARRAYSIZE(xTextFilter.InputBuf), ImGuiInputTextFlags_EscapeClearsAll | ImGuiInputTextFlags_EnterReturnsTrue)) {
        xTextFilter.Build();
        values.x = std::stof(xTextFilter.InputBuf);
        isValueModified = true;
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
    std::snprintf(yPosition, sizeof(yPosition), "%.10f", values.y);
    if (ImGui::InputTextWithHint("##YTextFilter", yPosition, yTextFilter.InputBuf, IM_ARRAYSIZE(yTextFilter.InputBuf), ImGuiInputTextFlags_EscapeClearsAll | ImGuiInputTextFlags_EnterReturnsTrue)) {
        yTextFilter.Build();
        values.y = std::stof(yTextFilter.InputBuf);
        isValueModified = true;
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
    std::snprintf(zPosition, sizeof(zPosition), "%.10f", values.z);
    if (ImGui::InputTextWithHint("##ZTextFilter", zPosition, zTextFilter.InputBuf, IM_ARRAYSIZE(zTextFilter.InputBuf), ImGuiInputTextFlags_EscapeClearsAll | ImGuiInputTextFlags_EnterReturnsTrue)) {
        zTextFilter.Build();
        values.z = std::stof(zTextFilter.InputBuf);
        isValueModified = true;
    }

    ImGui::PopStyleVar();
    ImGui::Columns(1);
    ImGui::PopID();

    return isValueModified;
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
    _drawCameraComponent(registry);

    ImGui::Spacing();
    ImGui::Separator();

    // Add Component button
    /*const float buttonWidth{ImGui::GetContentRegionAvail().x};
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
    }*/

    ImGui::End();
}

void InspectorPanel::_drawTransformComponent(entt::registry& registry) const {
    if (!registry.all_of<RelativeTransform>(_selectedEntity)) {
        return;
    }

    // TODO: this (along with DrawAndUpdateVec3Widget) is bad and need to be refactored.
    const ImGuiTreeNodeFlags flags{
        ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
        ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowOverlap |
        ImGuiTreeNodeFlags_FramePadding};

    if (ImGui::TreeNodeEx("##Transform", flags, "Transform")) {
        // Position
        auto& relativeTransform{registry.get<RelativeTransform>(_selectedEntity).Value};
        auto& transformFlag{registry.get<TransformDirtyFlag>(_selectedEntity)};
        if (DrawAndUpdateVec3Widget("Position", relativeTransform.Position)) {
            transformFlag.ShouldUpdateTransform = true;
        }

        glm::vec3 euler{glm::degrees(glm::eulerAngles(relativeTransform.Rotation))};
        if (DrawAndUpdateVec3Widget("Rotation", euler)) {
            relativeTransform.Rotation = glm::quat{glm::radians(euler)};
            transformFlag.ShouldUpdateTransform = true;
        }

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, 100.f);
        ImGui::Text("Scale");
        ImGui::NextColumn();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::DragFloat("##Scale", &relativeTransform.Scale, 0.01f, 0.001f, 100.f, "%.3f")) {
            transformFlag.ShouldUpdateTransform = true;
        }
        ImGui::Columns(1);

        ImGui::TreePop();
    }
}

void InspectorPanel::_drawMeshComponent(entt::registry& registry) const {
    if (!registry.any_of<MeshNodeTag>(_selectedEntity)) {
        return;
    }

    const ImGuiTreeNodeFlags flags{
        ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
        ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding};

    if (ImGui::TreeNodeEx("##Mesh", flags, "Mesh")) {
        if (auto* const meshName{registry.try_get<Name>(_selectedEntity)}) {
            ImGui::Text("Mesh ID : %s", meshName->Value.c_str());
        }

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
        DrawAndUpdateVec3Widget("Direction", directionalLight.Direction);
        ImGui::ColorEdit3("Ambient",  &directionalLight.Ambient.x);
        ImGui::ColorEdit3("Diffuse",  &directionalLight.Diffuse.x);
        ImGui::ColorEdit3("Specular", &directionalLight.Specular.x);
        ImGui::TreePop();
    }
}

void InspectorPanel::_drawCameraComponent(entt::registry& registry) const {
    if (!registry.all_of<Camera>(_selectedEntity)) {
        return;
    }

    const ImGuiTreeNodeFlags flags{
        ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
        ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding};

    if (ImGui::TreeNodeEx("##Camera", flags, "Camera")) {
        ImGui::Text("Camera");
        ImGui::TreePop();
    }

    _drawTransformComponent(registry);
}

