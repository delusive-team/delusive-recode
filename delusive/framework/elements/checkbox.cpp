#include "../settings/functions.h"

bool c_gui::checkbox(std::string_view label, bool* callback)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label.data());

    const ImVec2 pos = window->DC.CursorPos;
    const float width = GetContentRegionAvail().x;
    const ImRect rect(pos, pos + ImVec2(width, elements->widgets.checkbox_size.y));
    const ImRect clickable(rect.Min, rect.Min + elements->widgets.checkbox_size);
    const ImRect text(clickable.Min, clickable.Max + ImVec2(elements->widgets.padding.x + var->font.tahoma->CalcTextSizeA(var->font.tahoma->FontSize, FLT_MAX, -1.f, label.data()).x, 0));

    ItemSize(rect, style.FramePadding.y);
    if (!ItemAdd(rect, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(text, id, &hovered, &held);
    if (pressed)
        *callback = !(*callback);

    draw->fade_rect_filled(window->DrawList, clickable.Min + ImVec2(2, 2), clickable.Max - ImVec2(2, 2), draw->get_clr(clr->window.background_two), draw->get_clr(clr->window.background_one), fade_direction::vertically);
    if (*callback)
        draw->fade_rect_filled(window->DrawList, clickable.Min + ImVec2(2, 2), clickable.Max - ImVec2(2, 2), draw->get_clr(clr->accent), draw->get_clr({ clr->accent.Value.x - 0.2f, clr->accent.Value.y - 0.2f, clr->accent.Value.z - 0.2f, 1.f}), fade_direction::vertically);
    draw->rect(window->DrawList, clickable.Min, clickable.Max, draw->get_clr(var->window.hover_hightlight && hovered ? clr->accent : clr->widgets.stroke_two));
    draw->rect(window->DrawList, clickable.Min + ImVec2(1, 1), clickable.Max - ImVec2(1, 1), draw->get_clr(clr->window.stroke));

    draw->text_clipped_outline(window->DrawList, var->font.tahoma, ImVec2(clickable.Max.x + elements->widgets.padding.x, rect.Min.y), rect.Max, draw->get_clr(clr->widgets.text), label.data(), NULL, NULL, ImVec2(0.f, 0.5f));

    return pressed;
}

bool c_gui::checkbox(std::string_view label, bool* callback, int* key, int* mode)
{
    if (key != nullptr)
    {
        auto it = std::find_if(registered_keybinds.begin(), registered_keybinds.end(),
            [key](const keybind_reg_t& b) { return b.key == key; });

        if (it == registered_keybinds.end()) {
            registered_keybinds.push_back({ std::string(label), key, mode, callback, 0.f });
        }
        else {
            it->display_name = std::string(label);
            it->state = callback;
        }
    }

    float y_pos = GetCursorPosY();
    bool pressed = gui->checkbox(label, callback); 

    ImVec2 stored_pos = GetCursorPos();

    set_cursor_pos(ImVec2(GetWindowWidth() - elements->widgets.key_size.x - GetStyle().WindowPadding.x, y_pos));

    gui->keybind((std::string("##bind_") + std::string(label)).c_str(), key, mode);

    set_cursor_pos(stored_pos);
    return pressed;
}