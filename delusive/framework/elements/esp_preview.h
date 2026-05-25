#pragma once

#include "../settings/functions.h"
#include "../../configs/configs.h"
#include "../../thirdparty/imgui/imgui.h"
#include "../../thirdparty/imgui/imgui_internal.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

extern std::string g_discord_username;

namespace esp_preview
{
    enum anchor_t : int
    {
        anchor_top = 0,
        anchor_bottom = 1,
        anchor_left = 2,
        anchor_right = 3
    };

    enum item_id : int
    {
        item_name,
        item_health,
        item_distance,
        item_weapon,
        item_flag_visible,
        item_flag_zooming,
        item_flag_safezone,
        item_flag_inside,
        item_flag_mount,
        item_flag_sleep,
        item_flag_team,
        item_count
    };

    struct item_t
    {
        const char* label{};
        const char* preview{};
        bool* enabled{};
        int flag_index = -1;

        float* ox{};
        float* oy{};
        float* size{};
        int* anchor{};
        ImColor* color{};
        int* text_case{};
        int* font{};
        int* style{};
        bool is_bar{};
        int order{};

        ImVec2 base{};
        ImVec2 target{};
        ImVec2 pos{};
        ImVec2 extent{};
        ImVec2 drag_delta{};
        bool dragging = false;
        bool inited = false;
        float hover = 0.f;
    };

    struct state_t
    {
        std::array<item_t, item_count> items{};
        bool inited = false;
        int active = -1;
        ImVec2 last_canvas{};
        bool canvas_valid = false;
        float box_hover = 0.f;
        float skel_hover = 0.f;
    };

    inline state_t& state()
    {
        static state_t st;
        return st;
    }

    static inline ImU32 to_u32(const ImColor& c, float alpha = 1.f)
    {
        ImVec4 v = c.Value;
        v.w *= alpha;
        return ImGui::ColorConvertFloat4ToU32(v);
    }

    static inline float lerp(float a, float b, float t)
    {
        return a + (b - a) * ImClamp(t, 0.f, 1.f);
    }

    static inline ImVec2 lerp(ImVec2 a, ImVec2 b, float t)
    {
        return ImVec2(lerp(a.x, b.x, t), lerp(a.y, b.y, t));
    }

    static inline bool flag_selected(int idx)
    {
        if (!config::esp::players_flags.value)
            return false;

        for (int v : config::esp::players_flags_list.value) {
            if (v == idx)
                return true;
        }

        switch (idx) {
        case 0: return config::esp::players_flag_visible.value;
        case 1: return config::esp::players_flag_zooming.value;
        case 2: return config::esp::players_flag_safezone.value;
        case 3: return config::esp::players_flag_inside.value;
        case 4: return config::esp::players_flag_mount.value;
        case 5: return config::esp::players_flag_sleep.value;
        case 6: return config::esp::players_flag_team.value;
        default: return false;
        }
    }

    static inline bool item_enabled(const item_t& item)
    {
        if (item.flag_index >= 0)
            return flag_selected(item.flag_index);
        return item.enabled && *item.enabled;
    }

    static inline std::string apply_case(std::string text, int mode)
    {
        if (mode == 1)
            std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c) { return (char)std::toupper(c); });
        else if (mode == 2)
            std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c) { return (char)std::tolower(c); });
        return text;
    }

    static inline ImFont* get_font(int mode)
    {
        if (mode == 1 && ImGui::GetIO().FontDefault)
            return ImGui::GetIO().FontDefault;
        return var->font.tahoma ? var->font.tahoma : ImGui::GetFont();
    }

    static inline std::string item_text(const item_t& item)
    {
        std::string text;
        if (item.flag_index >= 0) {
            text = item.preview ? item.preview : "";
        }
        else if (item.label == std::string("Name")) {
            text = g_discord_username.empty() ? "Player Name" : g_discord_username;
        }
        else if (item.label == std::string("Distance")) {
            text = "42m";
        }
        else if (item.label == std::string("Weapon")) {
            text = config::esp::players_weapon_type.value == 0 ? "Assault Rifle" :
                (config::esp::players_weapon_type.value == 1 ? "[AK]" : "AK-47");
        }

        return item.text_case ? apply_case(text, *item.text_case) : text;
    }

    static inline int nearest_anchor(ImVec2 p, ImVec2 box_min, ImVec2 box_max)
    {
        const float distances[] = {
            fabsf(p.y - box_min.y),
            fabsf(p.y - box_max.y),
            fabsf(p.x - box_min.x),
            fabsf(p.x - box_max.x)
        };

        int best = anchor_top;
        for (int i = 1; i < 4; ++i) {
            if (distances[i] < distances[best])
                best = i;
        }
        return best;
    }

    static inline int clean_anchor(int value, int fallback)
    {
        if (value >= anchor_top && value <= anchor_right)
            return value;
        return fallback;
    }

    static inline void init_items()
    {
        auto& it = state().items;
        it[item_name] = { "Name", nullptr, &config::esp::players_name.value, -1, &config::esp::prev_name_ox.value, &config::esp::prev_name_oy.value, &config::esp::prev_name_sz.value, &config::esp::prev_name_align.value, &config::esp::players_name_color.value, &config::esp::players_name_case.value, &config::esp::players_name_font.value, &config::esp::players_name_style.value, false, 0 };
        it[item_health] = { "HealthBar", nullptr, &config::esp::players_health.value, -1, &config::esp::prev_health_ox.value, &config::esp::prev_health_oy.value, &config::esp::prev_health_sz.value, &config::esp::prev_health_align.value, &config::esp::players_health_color.value, nullptr, nullptr, nullptr, true, 1 };
        it[item_distance] = { "Distance", nullptr, &config::esp::players_distance.value, -1, &config::esp::prev_dist_ox.value, &config::esp::prev_dist_oy.value, &config::esp::prev_dist_sz.value, &config::esp::prev_dist_align.value, &config::esp::players_distance_color.value, &config::esp::players_distance_case.value, &config::esp::players_distance_font.value, &config::esp::players_distance_style.value, false, 2 };
        it[item_weapon] = { "Weapon", nullptr, &config::esp::players_weapon.value, -1, &config::esp::prev_weap_ox.value, &config::esp::prev_weap_oy.value, &config::esp::prev_weap_sz.value, &config::esp::prev_weap_align.value, &config::esp::players_weapon_color.value, &config::esp::players_weapon_case.value, &config::esp::players_weapon_font.value, &config::esp::players_weapon_style.value, false, 3 };

        it[item_flag_visible] = { "Flag Visible", "Visible", nullptr, 0, &config::esp::prev_flag_visible_ox.value, &config::esp::prev_flag_visible_oy.value, &config::esp::prev_flag_visible_sz.value, &config::esp::players_flags_visible_pos.value, &config::esp::prev_flag_visible_color.value, &config::esp::prev_flag_visible_case.value, &config::esp::prev_flag_visible_font.value, &config::esp::prev_flag_visible_style.value, false, 4 };
        it[item_flag_zooming] = { "Flag Zooming", "Zooming", nullptr, 1, &config::esp::prev_flag_zooming_ox.value, &config::esp::prev_flag_zooming_oy.value, &config::esp::prev_flag_zooming_sz.value, &config::esp::players_flags_zooming_pos.value, &config::esp::prev_flag_zooming_color.value, &config::esp::prev_flag_zooming_case.value, &config::esp::prev_flag_zooming_font.value, &config::esp::prev_flag_zooming_style.value, false, 5 };
        it[item_flag_safezone] = { "Flag Safezone", "SafeZone", nullptr, 2, &config::esp::prev_flag_safezone_ox.value, &config::esp::prev_flag_safezone_oy.value, &config::esp::prev_flag_safezone_sz.value, &config::esp::players_flags_safezone_pos.value, &config::esp::prev_flag_safezone_color.value, &config::esp::prev_flag_safezone_case.value, &config::esp::prev_flag_safezone_font.value, &config::esp::prev_flag_safezone_style.value, false, 6 };
        it[item_flag_inside] = { "Flag Inside", "Inside", nullptr, 3, &config::esp::prev_flag_inside_ox.value, &config::esp::prev_flag_inside_oy.value, &config::esp::prev_flag_inside_sz.value, &config::esp::players_flags_inside_pos.value, &config::esp::prev_flag_inside_color.value, &config::esp::prev_flag_inside_case.value, &config::esp::prev_flag_inside_font.value, &config::esp::prev_flag_inside_style.value, false, 7 };
        it[item_flag_mount] = { "Flag Mount", "Mount", nullptr, 4, &config::esp::prev_flag_mount_ox.value, &config::esp::prev_flag_mount_oy.value, &config::esp::prev_flag_mount_sz.value, &config::esp::players_flags_mount_pos.value, &config::esp::prev_flag_mount_color.value, &config::esp::prev_flag_mount_case.value, &config::esp::prev_flag_mount_font.value, &config::esp::prev_flag_mount_style.value, false, 8 };
        it[item_flag_sleep] = { "Flag Sleep", "Sleep", nullptr, 5, &config::esp::prev_flag_sleep_ox.value, &config::esp::prev_flag_sleep_oy.value, &config::esp::prev_flag_sleep_sz.value, &config::esp::players_flags_sleep_pos.value, &config::esp::prev_flag_sleep_color.value, &config::esp::prev_flag_sleep_case.value, &config::esp::prev_flag_sleep_font.value, &config::esp::prev_flag_sleep_style.value, false, 9 };
        it[item_flag_team] = { "Flag Team", "Team", nullptr, 6, &config::esp::prev_flag_team_ox.value, &config::esp::prev_flag_team_oy.value, &config::esp::prev_flag_team_sz.value, &config::esp::players_flags_team_pos.value, &config::esp::prev_flag_team_color.value, &config::esp::prev_flag_team_case.value, &config::esp::prev_flag_team_font.value, &config::esp::prev_flag_team_style.value, false, 10 };
    }

    static inline void render_text(ImDrawList* dl, const item_t& item, ImVec2 pos, float size, ImU32 col, const char* text)
    {
        ImFont* font = get_font(item.font ? *item.font : 0);
        const int style = item.style ? *item.style : 1;

        if (style == 2 || style == 3)
            ::draw->text(dl, font, size, pos + ImVec2(1.f, 1.f), IM_COL32(0, 0, 0, 190), text);

        if (style == 1 || style == 3)
            ::draw->text_outline(dl, font, size, pos, col, text);
        else
            ::draw->text(dl, font, size, pos, col, text);
    }

    static inline void draw_box(ImDrawList* dl, ImVec2 box_min, ImVec2 box_max)
    {
        if (!config::esp::players_box.value)
            return;

        if (config::esp::players_box_fill.value)
            ::draw->rect_filled(dl, box_min, box_max, to_u32(config::esp::players_box_fill_color.value));

        const ImU32 col = to_u32(config::esp::players_box_color.value, 0.85f);
        if (config::esp::players_box_type.value == 1) {
            const float len = (box_max.x - box_min.x) / 3.5f;
            ::draw->line(dl, box_min, ImVec2(box_min.x + len, box_min.y), col, 1.5f);
            ::draw->line(dl, box_min, ImVec2(box_min.x, box_min.y + len), col, 1.5f);
            ::draw->line(dl, ImVec2(box_max.x, box_min.y), ImVec2(box_max.x - len, box_min.y), col, 1.5f);
            ::draw->line(dl, ImVec2(box_max.x, box_min.y), ImVec2(box_max.x, box_min.y + len), col, 1.5f);
            ::draw->line(dl, ImVec2(box_min.x, box_max.y), ImVec2(box_min.x + len, box_max.y), col, 1.5f);
            ::draw->line(dl, ImVec2(box_min.x, box_max.y), ImVec2(box_min.x, box_max.y - len), col, 1.5f);
            ::draw->line(dl, box_max, ImVec2(box_max.x - len, box_max.y), col, 1.5f);
            ::draw->line(dl, box_max, ImVec2(box_max.x, box_max.y - len), col, 1.5f);
        }
        else {
            ::draw->rect(dl, box_min, box_max, col, 0.f, 0, 1.2f);
        }
    }

    static inline void draw_skeleton(ImDrawList* dl, ImVec2 box_min, ImVec2 box_max, float scale)
    {
        if (!config::esp::players_skeleton.value)
            return;

        const float cx = (box_min.x + box_max.x) * 0.5f;
        const float h = box_max.y - box_min.y;
        const ImU32 col = to_u32(config::esp::players_skeleton_color.value);
        const float th = ImMax(1.f, config::esp::players_skeleton_thickness.value * scale);

        ImVec2 head(cx, box_min.y + h * 0.08f);
        ImVec2 neck(cx, box_min.y + h * 0.18f);
        ImVec2 pelvis(cx, box_min.y + h * 0.55f);
        ImVec2 lsh(cx - h * 0.10f, neck.y);
        ImVec2 rsh(cx + h * 0.10f, neck.y);
        ImVec2 lel(cx - h * 0.14f, box_min.y + h * 0.34f);
        ImVec2 rel(cx + h * 0.14f, box_min.y + h * 0.34f);
        ImVec2 lhd(cx - h * 0.13f, box_min.y + h * 0.50f);
        ImVec2 rhd(cx + h * 0.13f, box_min.y + h * 0.50f);
        ImVec2 lkn(cx - h * 0.08f, box_min.y + h * 0.78f);
        ImVec2 rkn(cx + h * 0.08f, box_min.y + h * 0.78f);
        ImVec2 lft(cx - h * 0.10f, box_max.y - h * 0.02f);
        ImVec2 rft(cx + h * 0.10f, box_max.y - h * 0.02f);

        const ImVec2 bones[][2] = {
            { head, neck }, { neck, pelvis }, { neck, lsh }, { neck, rsh },
            { lsh, lel }, { rsh, rel }, { lel, lhd }, { rel, rhd },
            { pelvis, lkn }, { pelvis, rkn }, { lkn, lft }, { rkn, rft }
        };

        for (auto& b : bones)
            ::draw->line(dl, b[0], b[1], col, th);

        if (config::esp::players_skeleton_joints.value) {
            const ImVec2 joints[] = { head, neck, pelvis, lsh, rsh, lel, rel, lhd, rhd, lkn, rkn, lft, rft };
            for (auto& p : joints)
                ::draw->circle_filled(dl, p, th + 1.5f, IM_COL32(255, 255, 255, 230), 12);
        }
    }

    static inline void begin_preview_popup()
    {
        ImGui::SetNextWindowSizeConstraints(ImVec2(330.f, 0.f), ImVec2(360.f, 560.f));
        gui->push_style_color(ImGuiCol_PopupBg, ::draw->get_clr(clr->window.background_one));
        gui->push_style_color(ImGuiCol_Border, ::draw->get_clr(clr->window.stroke));
        gui->push_font(var->font.tahoma);
        gui->push_style_var(ImGuiStyleVar_WindowPadding, ImVec2(14, 14));
        gui->push_style_var(ImGuiStyleVar_ItemSpacing, ImVec2(8, 9));
        gui->push_style_var(ImGuiStyleVar_WindowRounding, 12.f);
        ImGui::PushItemWidth(170.f);
    }

    static inline void end_preview_popup()
    {
        ImGui::PopItemWidth();
        gui->pop_style_var(3);
        gui->pop_font();
        gui->pop_style_color(2);
    }

    static inline void draw_hue_bar(ImDrawList* dl, ImVec2 min, ImVec2 max)
    {
        constexpr int segments = 6;
        for (int i = 0; i < segments; ++i) {
            const float h0 = (float)i / segments;
            const float h1 = (float)(i + 1) / segments;
            float r0, g0, b0, r1, g1, b1;
            ImGui::ColorConvertHSVtoRGB(h0, 1.f, 1.f, r0, g0, b0);
            ImGui::ColorConvertHSVtoRGB(h1, 1.f, 1.f, r1, g1, b1);
            const float x0 = ImLerp(min.x, max.x, h0);
            const float x1 = ImLerp(min.x, max.x, h1);
            dl->AddRectFilledMultiColor(
                ImVec2(x0, min.y), ImVec2(x1, max.y),
                IM_COL32((int)(r0 * 255), (int)(g0 * 255), (int)(b0 * 255), 255),
                IM_COL32((int)(r1 * 255), (int)(g1 * 255), (int)(b1 * 255), 255),
                IM_COL32((int)(r1 * 255), (int)(g1 * 255), (int)(b1 * 255), 255),
                IM_COL32((int)(r0 * 255), (int)(g0 * 255), (int)(b0 * 255), 255));
        }
    }

    static inline void draw_alpha_checker(ImDrawList* dl, ImVec2 min, ImVec2 max)
    {
        const float cell = 6.f;
        for (float y = min.y; y < max.y; y += cell) {
            for (float x = min.x; x < max.x; x += cell) {
                const bool alt = ((int)((x - min.x) / cell) + (int)((y - min.y) / cell)) & 1;
                dl->AddRectFilled(ImVec2(x, y), ImVec2(ImMin(x + cell, max.x), ImMin(y + cell, max.y)), alt ? IM_COL32(205, 205, 205, 255) : IM_COL32(245, 245, 245, 255));
            }
        }
    }

    static inline void neverlose_color_picker(const char* id, ImColor& color)
    {
        ImGui::PushID(id);

        float h, s, v;
        ImGui::ColorConvertRGBtoHSV(color.Value.x, color.Value.y, color.Value.z, h, s, v);
        if (s <= 0.f)
            h = 0.f;

        ImDrawList* dl = ImGui::GetWindowDrawList();
        const float width = ImGui::GetContentRegionAvail().x;
        const float sv_size = ImMin(width, 300.f);
        ImVec2 sv_min = ImGui::GetCursorScreenPos();
        ImVec2 sv_max = sv_min + ImVec2(sv_size, sv_size);

        float hr, hg, hb;
        ImGui::ColorConvertHSVtoRGB(h, 1.f, 1.f, hr, hg, hb);
        dl->AddRectFilledMultiColor(sv_min, sv_max, IM_COL32_WHITE, IM_COL32((int)(hr * 255), (int)(hg * 255), (int)(hb * 255), 255), IM_COL32((int)(hr * 255), (int)(hg * 255), (int)(hb * 255), 255), IM_COL32_WHITE);
        dl->AddRectFilledMultiColor(sv_min, sv_max, IM_COL32(0, 0, 0, 0), IM_COL32(0, 0, 0, 0), IM_COL32_BLACK, IM_COL32_BLACK);
        dl->AddRect(sv_min, sv_max, IM_COL32(255, 255, 255, 22), 8.f);

        ImVec2 marker = ImVec2(ImLerp(sv_min.x, sv_max.x, s), ImLerp(sv_max.y, sv_min.y, v));
        dl->AddCircleFilled(marker, 5.f, IM_COL32_WHITE, 16);
        dl->AddCircle(marker, 6.5f, IM_COL32(0, 0, 0, 210), 16, 1.5f);

        ImGui::InvisibleButton("sv", ImVec2(sv_size, sv_size));
        if (ImGui::IsItemActive()) {
            ImVec2 m = ImGui::GetIO().MousePos;
            s = ImClamp((m.x - sv_min.x) / sv_size, 0.f, 1.f);
            v = 1.f - ImClamp((m.y - sv_min.y) / sv_size, 0.f, 1.f);
            ImGui::ColorConvertHSVtoRGB(h, s, v, color.Value.x, color.Value.y, color.Value.z);
        }

        ImGui::Dummy(ImVec2(0, 8));

        ImVec2 hue_min = ImGui::GetCursorScreenPos();
        ImVec2 hue_max = hue_min + ImVec2(sv_size, 14.f);
        draw_hue_bar(dl, hue_min, hue_max);
        dl->AddRect(hue_min, hue_max, IM_COL32(0, 0, 0, 180), 3.f);
        const float hx = ImLerp(hue_min.x, hue_max.x, h);
        dl->AddCircleFilled(ImVec2(hx, (hue_min.y + hue_max.y) * 0.5f), 8.f, IM_COL32_WHITE, 16);
        dl->AddCircle(ImVec2(hx, (hue_min.y + hue_max.y) * 0.5f), 9.5f, IM_COL32(0, 0, 0, 190), 16, 1.3f);
        ImGui::InvisibleButton("hue", ImVec2(sv_size, 14.f));
        if (ImGui::IsItemActive()) {
            h = ImClamp((ImGui::GetIO().MousePos.x - hue_min.x) / sv_size, 0.f, 1.f);
            ImGui::ColorConvertHSVtoRGB(h, s, v, color.Value.x, color.Value.y, color.Value.z);
        }

        ImVec2 alpha_min = ImGui::GetCursorScreenPos() + ImVec2(0, 8);
        ImVec2 alpha_max = alpha_min + ImVec2(sv_size, 14.f);
        ImGui::SetCursorScreenPos(alpha_min);
        draw_alpha_checker(dl, alpha_min, alpha_max);
        const ImU32 c0 = IM_COL32((int)(color.Value.x * 255), (int)(color.Value.y * 255), (int)(color.Value.z * 255), 0);
        const ImU32 c1 = IM_COL32((int)(color.Value.x * 255), (int)(color.Value.y * 255), (int)(color.Value.z * 255), 255);
        dl->AddRectFilledMultiColor(alpha_min, alpha_max, c0, c1, c1, c0);
        dl->AddRect(alpha_min, alpha_max, IM_COL32(0, 0, 0, 180), 3.f);
        const float ax = ImLerp(alpha_min.x, alpha_max.x, color.Value.w);
        dl->AddCircleFilled(ImVec2(ax, (alpha_min.y + alpha_max.y) * 0.5f), 8.f, IM_COL32_WHITE, 16);
        dl->AddCircle(ImVec2(ax, (alpha_min.y + alpha_max.y) * 0.5f), 9.5f, IM_COL32(0, 0, 0, 190), 16, 1.3f);
        ImGui::InvisibleButton("alpha", ImVec2(sv_size, 14.f));
        if (ImGui::IsItemActive())
            color.Value.w = ImClamp((ImGui::GetIO().MousePos.x - alpha_min.x) / sv_size, 0.f, 1.f);

        ImGui::Dummy(ImVec2(0, 9));

        const int r = (int)ImClamp(color.Value.x * 255.f, 0.f, 255.f);
        const int g = (int)ImClamp(color.Value.y * 255.f, 0.f, 255.f);
        const int b = (int)ImClamp(color.Value.z * 255.f, 0.f, 255.f);
        const int a = (int)ImClamp(color.Value.w * 255.f, 0.f, 255.f);
        char hex[16];
        std::snprintf(hex, sizeof(hex), "#%02X%02X%02X%02X", r, g, b, a);

        ImVec2 row_min = ImGui::GetCursorScreenPos();
        ImVec2 row_max = row_min + ImVec2(sv_size, 34.f);
        ::draw->rect_filled(dl, row_min, row_max, IM_COL32(16, 17, 24, 170), 5.f);
        ::draw->rect(dl, row_min, row_max, IM_COL32(255, 255, 255, 14), 5.f);
        dl->AddRectFilled(row_min + ImVec2(12, 9), row_min + ImVec2(32, 29), to_u32(color), 3.f);
        ::draw->text(dl, var->font.tahoma, 13.f, row_min + ImVec2(42, 9), IM_COL32(255, 255, 255, 215), hex);

        char alpha_text[16];
        std::snprintf(alpha_text, sizeof(alpha_text), "%d%%", (int)(color.Value.w * 100.f + 0.5f));
        const float aw = var->font.tahoma->CalcTextSizeA(13.f, FLT_MAX, 0.f, alpha_text).x;
        ::draw->text(dl, var->font.tahoma, 13.f, ImVec2(row_max.x - aw - 12.f, row_min.y + 9.f), IM_COL32(255, 255, 255, 170), alpha_text);
        ImGui::Dummy(ImVec2(sv_size, 34.f));

        ImGui::PopID();
    }

    static inline void draw_context(item_t& item)
    {
        char id[64];
        std::snprintf(id, sizeof(id), "##esp_ctx_%s", item.label);

        begin_preview_popup();
        if (ImGui::BeginPopup(id)) {
            ImGui::TextColored(clr->accent, "%s", item.label);
            ImGui::Separator();

            if (item.color)
                neverlose_color_picker(item.label, *item.color);

            const char* anchors[] = { "Top", "Bottom", "Left", "Right" };
            gui->dropdown("Position", item.anchor, anchors, IM_ARRAYSIZE(anchors));

            if (!item.is_bar) {
                const char* fonts[] = { "Default", "Tahoma" };
                const char* styles[] = { "Default", "Outline", "Shadow", "Shadow + Outline" };
                const char* cases[] = { "Default", "UpperCase", "LowerCase" };

                if (item.size)
                    gui->slider_float("Font Size", item.size, 8.f, 24.f, false, "%.1f");
                if (item.font)
                    gui->dropdown("Font", item.font, fonts, IM_ARRAYSIZE(fonts));
                if (item.text_case)
                    gui->dropdown("Case", item.text_case, cases, IM_ARRAYSIZE(cases));
                if (item.style)
                    gui->dropdown("Draw", item.style, styles, IM_ARRAYSIZE(styles));
            }
            else {
                if (item.size)
                    gui->slider_float("Thickness", item.size, 2.f, 10.f, false, "%.1f");
                const char* hp_pos[] = { "Inside", "Outside" };
                gui->dropdown("Health Text Pos", &config::esp::players_health_text_pos.value, hp_pos, IM_ARRAYSIZE(hp_pos));
            }

            if (item.label == std::string("Weapon")) {
                const char* types[] = { "Text", "Icons", "Flat Icons" };
                gui->dropdown("Weapon Type", &config::esp::players_weapon_type.value, types, IM_ARRAYSIZE(types));
            }

            if (gui->button("Reset Position")) {
                *item.ox = 0.f;
                *item.oy = 0.f;
            }

            ImGui::EndPopup();
        }
        end_preview_popup();
    }

    static inline void draw_box_context()
    {
        begin_preview_popup();
        if (ImGui::BeginPopup("##esp_ctx_box")) {
            ImGui::TextColored(clr->accent, "Box");
            ImGui::Separator();
            const char* box_types[] = { "Normal", "Corner" };
            gui->dropdown("Box Type", &config::esp::players_box_type.value, box_types, IM_ARRAYSIZE(box_types));
            gui->checkbox("Box Fill", &config::esp::players_box_fill.value);
            gui->label_color_edit("Box Color", (float*)&config::esp::players_box_color.value.Value);
            gui->label_color_edit("Fill Color", (float*)&config::esp::players_box_fill_color.value.Value);
            ImGui::EndPopup();
        }
        end_preview_popup();
    }

    static inline void draw_skeleton_context()
    {
        begin_preview_popup();
        if (ImGui::BeginPopup("##esp_ctx_skeleton")) {
            ImGui::TextColored(clr->accent, "Skeleton");
            ImGui::Separator();
            gui->slider_float("Thickness", &config::esp::players_skeleton_thickness.value, 1.f, 5.f, false, "%.1f");
            gui->checkbox("Joints", &config::esp::players_skeleton_joints.value);
            gui->checkbox("Fingers", &config::esp::players_skeleton_fingers.value);
            gui->label_color_edit("Color", (float*)&config::esp::players_skeleton_color.value.Value);
            ImGui::EndPopup();
        }
        end_preview_popup();
    }

    static inline ImVec2 base_for_anchor(int anchor, ImVec2 box_min, ImVec2 box_max, ImVec2 extent, float scale)
    {
        anchor = clean_anchor(anchor, anchor_top);
        switch (anchor) {
        case anchor_top:
            return ImVec2((box_min.x + box_max.x - extent.x) * 0.5f, box_min.y - extent.y - 6.f * scale);
        case anchor_bottom:
            return ImVec2((box_min.x + box_max.x - extent.x) * 0.5f, box_max.y + 6.f * scale);
        case anchor_left:
            return ImVec2(box_min.x - extent.x - 7.f * scale, (box_min.y + box_max.y - extent.y) * 0.5f);
        case anchor_right:
            return ImVec2(box_max.x + 7.f * scale, (box_min.y + box_max.y - extent.y) * 0.5f);
        default:
            return box_min;
        }
    }

    inline void draw(ImVec2 canvas_min, ImVec2 canvas_max)
    {
        auto& st = state();
        if (!st.inited) {
            init_items();
            st.inited = true;
        }

        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImGuiIO& io = ImGui::GetIO();
        const float cw = ImMax(1.f, canvas_max.x - canvas_min.x);
        const float ch = ImMax(1.f, canvas_max.y - canvas_min.y);
        const float ui_scale = ImClamp(ImMin(cw / 450.f, ch / 500.f), 0.75f, 1.25f);
        const float anim = ImClamp(io.DeltaTime * 18.f, 0.f, 1.f);

        if (!st.canvas_valid) {
            st.last_canvas = canvas_min;
            st.canvas_valid = true;
        }

        const ImVec2 canvas_delta = canvas_min - st.last_canvas;
        if (canvas_delta.x != 0.f || canvas_delta.y != 0.f) {
            for (auto& item : st.items) {
                item.pos += canvas_delta;
                item.target += canvas_delta;
                item.base += canvas_delta;
            }
            st.last_canvas = canvas_min;
        }

        ::draw->rect_filled_multi_color(
            dl,
            canvas_min,
            canvas_max,
            IM_COL32(8, 12, 18, 255),
            IM_COL32(8, 12, 18, 255),
            IM_COL32(19, 20, 26, 255),
            IM_COL32(13, 15, 21, 255));
        ::draw->rect(dl, canvas_min, canvas_max, IM_COL32(255, 255, 255, 24));

        const ImVec2 player = ImVec2(canvas_min.x + cw * 0.50f, canvas_min.y + ch * 0.52f);
        const float box_w = ImClamp(cw * 0.31f, 92.f, 128.f) * ui_scale;
        const float box_h = ImClamp(ch * 0.70f, 230.f, 330.f) * ui_scale;
        const ImVec2 box_min = ImVec2(player.x - box_w * 0.5f, player.y - box_h * 0.5f);
        const ImVec2 box_max = ImVec2(player.x + box_w * 0.5f, player.y + box_h * 0.5f);

        const bool popup_open = ImGui::IsPopupOpen(nullptr, ImGuiPopupFlags_AnyPopupId);
        const bool in_canvas = ImGui::IsMouseHoveringRect(canvas_min, canvas_max);

        ::draw->push_clip_rect(dl, canvas_min + ImVec2(1, 1), canvas_max - ImVec2(1, 1), true);

        draw_skeleton(dl, box_min, box_max, ui_scale);
        draw_box(dl, box_min, box_max);

        const ImRect skeleton_hit(ImVec2(box_min.x + box_w * 0.24f, box_min.y), ImVec2(box_max.x - box_w * 0.24f, box_max.y));
        const bool skeleton_hovered = !popup_open && in_canvas && config::esp::players_skeleton.value && ImGui::IsMouseHoveringRect(skeleton_hit.Min, skeleton_hit.Max);
        const bool box_hovered = !skeleton_hovered && !popup_open && in_canvas && ImGui::IsMouseHoveringRect(box_min, box_max);
        st.box_hover = lerp(st.box_hover, box_hovered ? 1.f : 0.f, anim);
        st.skel_hover = lerp(st.skel_hover, skeleton_hovered ? 1.f : 0.f, anim);

        if (st.box_hover > 0.01f)
            ::draw->rect(dl, box_min - ImVec2(3, 3), box_max + ImVec2(3, 3), IM_COL32(255, 255, 255, (int)(70.f * st.box_hover)), 0.f, 0, 1.f);
        if (st.skel_hover > 0.01f)
            ::draw->rect(dl, skeleton_hit.Min, skeleton_hit.Max, IM_COL32(255, 255, 255, (int)(60.f * st.skel_hover)), 0.f, 0, 1.f);

        if (box_hovered && ImGui::IsMouseClicked(1))
            ImGui::OpenPopup("##esp_ctx_box");
        if (skeleton_hovered && ImGui::IsMouseClicked(1))
            ImGui::OpenPopup("##esp_ctx_skeleton");

        std::array<item_t*, item_count> ordered{};
        for (int i = 0; i < item_count; ++i)
            ordered[i] = &st.items[i];
        std::sort(ordered.begin(), ordered.end(), [](const item_t* a, const item_t* b) { return a->order < b->order; });

        for (item_t* item : ordered) {
            if (!item_enabled(*item))
                continue;

            *item->anchor = clean_anchor(*item->anchor, item->is_bar ? anchor_left : anchor_bottom);
            const float logical_size = item->size ? *item->size : 10.f;

            if (item->is_bar) {
                const float thick = ImMax(2.f, logical_size * ui_scale);
                item->extent = (*item->anchor == anchor_top || *item->anchor == anchor_bottom)
                    ? ImVec2(box_w, thick)
                    : ImVec2(thick, box_h);
            }
            else {
                const std::string text = item_text(*item);
                item->extent = get_font(item->font ? *item->font : 0)->CalcTextSizeA(logical_size * ui_scale, FLT_MAX, 0.f, text.c_str());
            }

            item->base = base_for_anchor(*item->anchor, box_min, box_max, item->extent, ui_scale);
            item->target = item->base + ImVec2(*item->ox * ui_scale, *item->oy * ui_scale);

            if (!item->inited) {
                item->pos = item->target;
                item->inited = true;
            }
            else if (!item->dragging) {
                item->pos = lerp(item->pos, item->target, anim);
            }
        }

        for (int i = 0; i < item_count; ++i) {
            item_t& item = st.items[i];
            if (!item_enabled(item))
                continue;

            const ImVec2 pad = ImVec2(5.f, 4.f) * ui_scale;
            const bool hovered = !popup_open && st.active == -1 && in_canvas && ImGui::IsMouseHoveringRect(item.pos - pad, item.pos + item.extent + pad);
            item.hover = lerp(item.hover, (hovered || item.dragging) ? 1.f : 0.f, anim);

            if (item.hover > 0.01f) {
                ::draw->rect_filled(dl, item.pos - pad, item.pos + item.extent + pad, IM_COL32(255, 255, 255, (int)(24.f * item.hover)), 3.f);
                ::draw->rect(dl, item.pos - pad, item.pos + item.extent + pad, IM_COL32(255, 255, 255, (int)(95.f * item.hover)), 3.f);
            }

            if (hovered) {
                ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
                if (ImGui::IsMouseClicked(0)) {
                    st.active = i;
                    item.dragging = true;
                    item.drag_delta = io.MousePos - item.pos;
                }
                if (ImGui::IsMouseClicked(1)) {
                    char id[64];
                    std::snprintf(id, sizeof(id), "##esp_ctx_%s", item.label);
                    ImGui::OpenPopup(id);
                }
            }

            if (st.active == i) {
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
                if (ImGui::IsMouseDown(0)) {
                    const int new_anchor = nearest_anchor(io.MousePos, box_min, box_max);
                    *item.anchor = new_anchor;
                    item.base = base_for_anchor(new_anchor, box_min, box_max, item.extent, ui_scale);
                    item.pos = io.MousePos - item.drag_delta;
                    *item.ox = (item.pos.x - item.base.x) / ui_scale;
                    *item.oy = (item.pos.y - item.base.y) / ui_scale;
                }
                else {
                    item.dragging = false;
                    st.active = -1;
                }
            }

            if (item.dragging) {
                const int a = *item.anchor;
                if (a == anchor_top) ::draw->rect_filled(dl, ImVec2(box_min.x, box_min.y - 8.f), ImVec2(box_max.x, box_min.y - 3.f), ::draw->get_clr(clr->accent, 0.35f), 2.f);
                if (a == anchor_bottom) ::draw->rect_filled(dl, ImVec2(box_min.x, box_max.y + 3.f), ImVec2(box_max.x, box_max.y + 8.f), ::draw->get_clr(clr->accent, 0.35f), 2.f);
                if (a == anchor_left) ::draw->rect_filled(dl, ImVec2(box_min.x - 8.f, box_min.y), ImVec2(box_min.x - 3.f, box_max.y), ::draw->get_clr(clr->accent, 0.35f), 2.f);
                if (a == anchor_right) ::draw->rect_filled(dl, ImVec2(box_max.x + 3.f, box_min.y), ImVec2(box_max.x + 8.f, box_max.y), ::draw->get_clr(clr->accent, 0.35f), 2.f);
            }

            if (item.is_bar) {
                const bool vertical = item.extent.y > item.extent.x;
                ::draw->rect_filled(dl, item.pos, item.pos + item.extent, IM_COL32(10, 10, 10, 210));

                constexpr float hp = 0.50f;
                ImVec2 fill_min = item.pos;
                ImVec2 fill_max = item.pos + item.extent;
                if (vertical)
                    fill_min.y = fill_max.y - item.extent.y * hp;
                else
                    fill_max.x = fill_min.x + item.extent.x * hp;

                ::draw->rect_filled(dl, fill_min, fill_max, to_u32(*item.color));
                ::draw->rect(dl, item.pos, item.pos + item.extent, IM_COL32(0, 0, 0, 220));

                if (config::esp::players_health_text.value) {
                    const char* hp_text = "50";
                    const float font_size = ImMax(8.f, 9.f * ui_scale);
                    const ImVec2 ts = var->font.tahoma->CalcTextSizeA(font_size, FLT_MAX, 0.f, hp_text);
                    ImVec2 text_pos;
                    if (config::esp::players_health_text_pos.value == 0)
                        text_pos = item.pos + (item.extent - ts) * 0.5f;
                    else if (vertical)
                        text_pos = ImVec2(item.pos.x + item.extent.x * 0.5f - ts.x * 0.5f, fill_min.y - ts.y - 2.f);
                    else
                        text_pos = ImVec2(fill_max.x + 4.f, item.pos.y + item.extent.y * 0.5f - ts.y * 0.5f);
                    ::draw->text(dl, var->font.tahoma, font_size, text_pos, IM_COL32(255, 255, 255, 230), hp_text);
                }
            }
            else {
                const std::string text = item_text(item);
                render_text(dl, item, ImVec2(std::floor(item.pos.x), std::floor(item.pos.y)), *item.size * ui_scale, to_u32(*item.color), text.c_str());
            }

            draw_context(item);
        }

        ::draw->pop_clip_rect(dl);

        draw_box_context();
        draw_skeleton_context();

        const char* caption = "GG HYLI NET";
        const float caption_size = ImMax(9.f, 10.f * ui_scale);
        ::draw->text(dl, var->font.tahoma, caption_size, canvas_min + ImVec2(10.f, 8.f), IM_COL32(255, 255, 255, 90), caption);
    }
}
