#pragma once
#include <string>
#include <vector>
#include "../../thirdparty/imgui/imgui.h"
#include "../../configs/configs.h"
class c_variables
{
public:
    struct {
        ImGuiWindowFlags main_flags{
            ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoNav |
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_Tooltip };
        ImGuiWindowFlags flags{
            ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoNav |
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground };

        ImVec2 padding{ 0, 0 };
        ImVec2 spacing{ 5, 6 };
        float  border_size{ 0 };
        float  rounding{ 4 };
        float  width{ 0 };
        float  titlebar{ 20 };
        float  scrollbar_size{ 2 };

        // ── Все настраиваемые поля теперь — прокси к конфигу ──
        bool& shadow_enabled = config::menu::shadow_enabled.value;
        float& shadow_size = config::menu::shadow_size.value;
        float& shadow_alpha = config::menu::shadow_alpha.value;
        bool& hover_hightlight = config::menu::hover_highlight.value;
        int& dpi_scale_idx = config::menu::dpi_scale_idx.value;
        bool& lock_layout = config::menu::lock_layout.value;
    } window;

    struct {
        bool current_section[5]{};
        const char* section_icons[5] = { "A", "B", "E", "G", "C" };

        float menu_alpha{ 0 };
        bool  menu_opened{ true };

        int& menu_key = config::menu::menu_key.value;
    } gui;

    struct {
        ImFont* icons[2]{};
        ImFont* tahoma{ nullptr };
        ImFont* segoe_ui{ nullptr };
    } font;
};

inline c_variables* var = new c_variables();