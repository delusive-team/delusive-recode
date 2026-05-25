#pragma once
#include "../../thirdparty/imgui/imgui.h"
#include "../../configs/configs.h"

class c_colors
{
public:
    // accent теперь берётся из конфига через apply()
    ImColor accent{ 220, 220, 220, 255 };

    struct {
        ImColor background_one{ 18, 18, 18, 255 };
        ImColor background_two{ 26, 26, 26, 255 };
        ImColor stroke{ 45, 45, 45, 255 };
    } window;

    struct {
        ImColor stroke_two{ 22, 22, 22, 255 };
        ImColor text{ 210, 210, 210, 255 };
        ImColor text_inactive{ 100, 100, 100, 255 };
    } widgets;

    // Вызывать каждый фрейм после загрузки конфига
    void apply() {
        accent = config::menu::accent_color.value;
    }
};

inline c_colors* clr = new c_colors();