#pragma once

#include <imgui.h>

namespace Thingy {
    inline void SetupImGuiStyle(bool bStyleDark, float alpha) {
        ImGuiStyle& style = ImGui::GetStyle();

        style.Alpha = 1.0f;
        style.FrameRounding = 5.0f;
        style.PopupRounding = 5.0f;

        style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 0.94f);
        style.Colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        style.Colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
        style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
        style.Colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
        style.Colors[ImGuiCol_FrameBg] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
        style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
        style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
        style.Colors[ImGuiCol_TitleBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
        style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
        style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
        style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
        style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
        style.Colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
        style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        style.Colors[ImGuiCol_Button] = ImVec4(1.0f, 0.8f, 0.0f, 1.0f);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
        style.Colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
        style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
        style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        //style.Colors[ImGuiCol_Separator];
        //style.Colors[ImGuiCol_SeparatorHovered];
        //style.Colors[ImGuiCol_SeparatorActive];
        style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
        style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
        style.Colors[ImGuiCol_TabHovered];
        style.Colors[ImGuiCol_Tab];
        style.Colors[ImGuiCol_TabSelected];
        style.Colors[ImGuiCol_TabSelectedOverline];
        style.Colors[ImGuiCol_TabDimmed];
        style.Colors[ImGuiCol_TabDimmedSelected];
        style.Colors[ImGuiCol_TabDimmedSelectedOverline];
        //style.Colors[ImGuiCol_DockingPreview];
        //style.Colors[ImGuiCol_DockingEmptyBg];
        style.Colors[ImGuiCol_TableHeaderBg];
        style.Colors[ImGuiCol_TableBorderStrong];
        style.Colors[ImGuiCol_TableBorderLight];
        style.Colors[ImGuiCol_TableRowBg];
        style.Colors[ImGuiCol_TableRowBgAlt];
        style.Colors[ImGuiCol_TextLink];
        style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
        style.Colors[ImGuiCol_DragDropTarget];
        style.Colors[ImGuiCol_NavCursor];
        style.Colors[ImGuiCol_NavWindowingHighlight];
        style.Colors[ImGuiCol_NavWindowingDimBg];
        style.Colors[ImGuiCol_ModalWindowDimBg];

        if (bStyleDark) {
            for (int i = 0; i <= ImGuiCol_COUNT; i++) {
                ImVec4& col = style.Colors[i];
                float H, S, V;
                ImGui::ColorConvertRGBtoHSV(col.x, col.y, col.z, H, S, V);

                if (S < 0.1f)
                {
                    V = 1.0f - V;
                }
                ImGui::ColorConvertHSVtoRGB(H, S, V, col.x, col.y, col.z);
                if (col.w < 1.00f)
                {
                    col.w *= alpha;
                }
            }
        } else {
            for (int i = 0; i <= ImGuiCol_COUNT; i++) {
                ImVec4& col = style.Colors[i];
                if (col.w < 1.00f) {
                    col.x *= alpha;
                    col.y *= alpha;
                    col.z *= alpha;
                    col.w *= alpha;
                }
            }
        }
    }
}