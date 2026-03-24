#pragma once

#include "imgui.h"

/**
 * @brief Applies a dark color theme to ImGui.
 *
 * Call once after ImGui::CreateContext() and before the first frame.
 * Colour names match the ImGui master-branch enum (1.90+).
 */
namespace EditorStyle {

inline void ApplyDarkTheme() {
    ImGuiStyle& style{ImGui::GetStyle()};

    // ── Rounding / spacing ────────────────────────────────────────────────
    style.WindowRounding    = 4.f;
    style.ChildRounding     = 4.f;
    style.FrameRounding     = 3.f;
    style.PopupRounding     = 4.f;
    style.ScrollbarRounding = 3.f;
    style.GrabRounding      = 3.f;
    style.TabRounding       = 4.f;

    style.WindowPadding     = ImVec2{8.f,  8.f};
    style.FramePadding      = ImVec2{5.f,  3.f};
    style.ItemSpacing       = ImVec2{6.f,  4.f};
    style.ItemInnerSpacing  = ImVec2{4.f,  4.f};
    style.IndentSpacing     = 18.f;
    style.ScrollbarSize     = 12.f;
    style.GrabMinSize       = 10.f;

    // ── Colours ───────────────────────────────────────────────────────────
    ImVec4* c{style.Colors};

    c[ImGuiCol_Text]                  = ImVec4{0.92f, 0.92f, 0.92f, 1.00f};
    c[ImGuiCol_TextDisabled]          = ImVec4{0.50f, 0.50f, 0.50f, 1.00f};
    c[ImGuiCol_TextLink]              = ImVec4{0.26f, 0.59f, 0.98f, 1.00f};

    c[ImGuiCol_WindowBg]              = ImVec4{0.16f, 0.16f, 0.16f, 1.00f};
    c[ImGuiCol_ChildBg]               = ImVec4{0.14f, 0.14f, 0.14f, 1.00f};
    c[ImGuiCol_PopupBg]               = ImVec4{0.18f, 0.18f, 0.18f, 0.98f};

    c[ImGuiCol_Border]                = ImVec4{0.08f, 0.08f, 0.08f, 1.00f};
    c[ImGuiCol_BorderShadow]          = ImVec4{0.00f, 0.00f, 0.00f, 0.00f};

    c[ImGuiCol_FrameBg]               = ImVec4{0.22f, 0.22f, 0.22f, 1.00f};
    c[ImGuiCol_FrameBgHovered]        = ImVec4{0.28f, 0.28f, 0.28f, 1.00f};
    c[ImGuiCol_FrameBgActive]         = ImVec4{0.32f, 0.32f, 0.32f, 1.00f};

    c[ImGuiCol_TitleBg]               = ImVec4{0.10f, 0.10f, 0.10f, 1.00f};
    c[ImGuiCol_TitleBgActive]         = ImVec4{0.12f, 0.12f, 0.12f, 1.00f};
    c[ImGuiCol_TitleBgCollapsed]      = ImVec4{0.10f, 0.10f, 0.10f, 0.75f};

    c[ImGuiCol_MenuBarBg]             = ImVec4{0.12f, 0.12f, 0.12f, 1.00f};

    c[ImGuiCol_ScrollbarBg]           = ImVec4{0.12f, 0.12f, 0.12f, 1.00f};
    c[ImGuiCol_ScrollbarGrab]         = ImVec4{0.36f, 0.36f, 0.36f, 1.00f};
    c[ImGuiCol_ScrollbarGrabHovered]  = ImVec4{0.44f, 0.44f, 0.44f, 1.00f};
    c[ImGuiCol_ScrollbarGrabActive]   = ImVec4{0.52f, 0.52f, 0.52f, 1.00f};

    c[ImGuiCol_CheckMark]             = ImVec4{0.26f, 0.59f, 0.98f, 1.00f};

    c[ImGuiCol_SliderGrab]            = ImVec4{0.36f, 0.36f, 0.36f, 1.00f};
    c[ImGuiCol_SliderGrabActive]      = ImVec4{0.52f, 0.52f, 0.52f, 1.00f};

    c[ImGuiCol_Button]                = ImVec4{0.26f, 0.26f, 0.26f, 1.00f};
    c[ImGuiCol_ButtonHovered]         = ImVec4{0.34f, 0.34f, 0.34f, 1.00f};
    c[ImGuiCol_ButtonActive]          = ImVec4{0.20f, 0.20f, 0.20f, 1.00f};

    c[ImGuiCol_Header]                = ImVec4{0.26f, 0.26f, 0.26f, 1.00f};
    c[ImGuiCol_HeaderHovered]         = ImVec4{0.34f, 0.34f, 0.34f, 1.00f};
    c[ImGuiCol_HeaderActive]          = ImVec4{0.20f, 0.20f, 0.20f, 1.00f};

    c[ImGuiCol_Separator]             = ImVec4{0.08f, 0.08f, 0.08f, 1.00f};
    c[ImGuiCol_SeparatorHovered]      = ImVec4{0.26f, 0.59f, 0.98f, 0.78f};
    c[ImGuiCol_SeparatorActive]       = ImVec4{0.26f, 0.59f, 0.98f, 1.00f};

    c[ImGuiCol_ResizeGrip]            = ImVec4{0.26f, 0.59f, 0.98f, 0.20f};
    c[ImGuiCol_ResizeGripHovered]     = ImVec4{0.26f, 0.59f, 0.98f, 0.67f};
    c[ImGuiCol_ResizeGripActive]      = ImVec4{0.26f, 0.59f, 0.98f, 0.95f};

    // Tab colours — use the 1.90.9+ names (old names are kept as aliases)
    c[ImGuiCol_Tab]                        = ImVec4{0.18f, 0.18f, 0.18f, 1.00f};
    c[ImGuiCol_TabHovered]                 = ImVec4{0.30f, 0.30f, 0.30f, 1.00f};
    c[ImGuiCol_TabSelected]                = ImVec4{0.24f, 0.24f, 0.24f, 1.00f};
    c[ImGuiCol_TabSelectedOverline]        = ImVec4{0.26f, 0.59f, 0.98f, 1.00f};
    c[ImGuiCol_TabDimmed]                  = ImVec4{0.14f, 0.14f, 0.14f, 1.00f};
    c[ImGuiCol_TabDimmedSelected]          = ImVec4{0.20f, 0.20f, 0.20f, 1.00f};
    c[ImGuiCol_TabDimmedSelectedOverline]  = ImVec4{0.50f, 0.50f, 0.50f, 1.00f};

    c[ImGuiCol_PlotLines]             = ImVec4{0.61f, 0.61f, 0.61f, 1.00f};
    c[ImGuiCol_PlotLinesHovered]      = ImVec4{1.00f, 0.43f, 0.35f, 1.00f};
    c[ImGuiCol_PlotHistogram]         = ImVec4{0.90f, 0.70f, 0.00f, 1.00f};
    c[ImGuiCol_PlotHistogramHovered]  = ImVec4{1.00f, 0.60f, 0.00f, 1.00f};

    c[ImGuiCol_TableHeaderBg]         = ImVec4{0.19f, 0.19f, 0.20f, 1.00f};
    c[ImGuiCol_TableBorderStrong]     = ImVec4{0.31f, 0.31f, 0.35f, 1.00f};
    c[ImGuiCol_TableBorderLight]      = ImVec4{0.23f, 0.23f, 0.25f, 1.00f};
    c[ImGuiCol_TableRowBg]            = ImVec4{0.00f, 0.00f, 0.00f, 0.00f};
    c[ImGuiCol_TableRowBgAlt]         = ImVec4{1.00f, 1.00f, 1.00f, 0.06f};

    c[ImGuiCol_TextSelectedBg]        = ImVec4{0.26f, 0.59f, 0.98f, 0.35f};
    c[ImGuiCol_DragDropTarget]        = ImVec4{1.00f, 1.00f, 0.00f, 0.90f};
    c[ImGuiCol_NavHighlight]          = ImVec4{0.26f, 0.59f, 0.98f, 1.00f};
    c[ImGuiCol_NavWindowingHighlight] = ImVec4{1.00f, 1.00f, 1.00f, 0.70f};
    c[ImGuiCol_NavWindowingDimBg]     = ImVec4{0.80f, 0.80f, 0.80f, 0.20f};
    c[ImGuiCol_ModalWindowDimBg]      = ImVec4{0.80f, 0.80f, 0.80f, 0.35f};
}

} // namespace EditorStyle
