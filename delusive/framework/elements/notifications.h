#pragma once
#include "../../framework/settings/functions.h"
#include <string>
#include <vector>
#include <algorithm>

namespace notifications
{
    enum type_t { INFO = 0, SUCCESS, WARNING, ERROR_T };

    struct notification_t
    {
        std::string title;
        std::string message;
        type_t      type;
        float       duration;
        float       elapsed;
        float       anim_t;
        bool        closing;
    };

    inline std::vector<notification_t> list;

    static ImColor type_color(type_t t)
    {
        switch (t) {
        case SUCCESS:  return ImColor(60, 185, 100);
        case WARNING:  return ImColor(220, 160, 40);
        case ERROR_T:  return ImColor(210, 60, 60);
        default:       return clr->accent;
        }
    }

    static const char* type_label(type_t t)
    {
        switch (t) {
        case SUCCESS:  return "Success";
        case WARNING:  return "Warning";
        case ERROR_T:  return "Error";
        default:       return "Info";
        }
    }

    inline void push(const char* title, const char* message,
        type_t type = INFO, float duration = 3.5f)
    {
        if (list.size() >= 5)
            list.erase(list.begin());

        list.push_back({ title, message, type, duration, 0.f, 0.f, false });
    }

    static float ease_out(float t) { return 1.f - (1.f - t) * (1.f - t); }
    static float ease_in(float t) { return t * t; }

    inline void draw(ImVec2 wm_pos, float wm_w, float wm_h)
    {
        if (list.empty()) return;

        auto* renderer = ::draw;

        ImDrawList* bg = ImGui::GetBackgroundDrawList();
        ImGuiIO& io = ImGui::GetIO();

        const float fsz = var->font.tahoma->FontSize;
        const float pad = 6.f;
        const float hdl_w = 0.f;
        const float bar_h = fsz + pad * 2.f;
        const float msg_h = fsz + pad * 2.f;
        const float prog_h = 2.f;
        const float gap = 4.f;
        const float min_w = 160.f;
        const float max_w = wm_w;

        const bool wm_on_right = (wm_pos.x + wm_w * 0.5f) > io.DisplaySize.x * 0.5f;
        const bool wm_on_bottom = (wm_pos.y + wm_h * 0.5f) > io.DisplaySize.y * 0.5f;

        auto gc = [&](ImColor color, float alpha = -1.f) -> ImU32 {
            const float a = (alpha < 0.f ? color.Value.w : alpha);
            return renderer->get_clr(color, a);
            };

        auto truncate_text = [&](const char* text, float max_avail) -> std::string {
            const float full = var->font.tahoma->CalcTextSizeA(fsz, FLT_MAX, 0.f, text).x;
            if (full <= max_avail) return text;

            const float ew = var->font.tahoma->CalcTextSizeA(fsz, FLT_MAX, 0.f, "...").x;
            std::string s = text;
            while (!s.empty()) {
                s.pop_back();
                while (!s.empty() && (s.back() & 0xC0) == 0x80) s.pop_back();
                if (var->font.tahoma->CalcTextSizeA(fsz, FLT_MAX, 0.f, s.c_str()).x + ew <= max_avail) {
                    s += "...";
                    return s;
                }
            }
            return "...";
            };

        const float dt = io.DeltaTime;

        list.erase(std::remove_if(list.begin(), list.end(), [&](notification_t& n) {
            n.elapsed += dt;
            constexpr float anim_dur = 0.25f;
            if (!n.closing) {
                n.anim_t = ImMin(n.anim_t + dt / anim_dur, 1.f);
                if (n.elapsed >= n.duration)
                    n.closing = true;
            }
            else {
                n.anim_t = ImMax(n.anim_t - dt / anim_dur, 0.f);
                if (n.anim_t <= 0.f) return true;
            }
            return false;
            }), list.end());

        std::vector<float> heights(list.size());
        for (int i = 0; i < (int)list.size(); i++)
            heights[i] = bar_h + msg_h + prog_h;

        for (int i = 0; i < (int)list.size(); i++)
        {
            notification_t& n = list[i];

            const char* type_str = type_label(n.type);
            const float tsw = var->font.tahoma->CalcTextSizeA(fsz, FLT_MAX, 0.f, type_str).x;
            const float wdot = var->font.tahoma->CalcTextSizeA(fsz, FLT_MAX, 0.f, "|").x;
            const float titw = var->font.tahoma->CalcTextSizeA(fsz, FLT_MAX, 0.f, n.title.c_str()).x;
            const float msgw = var->font.tahoma->CalcTextSizeA(fsz, FLT_MAX, 0.f, n.message.c_str()).x;

            const float header_w = hdl_w + pad + tsw + pad * 0.5f + wdot + pad * 0.5f + titw + pad;
            const float body_w = hdl_w + pad + msgw + pad;
            const float n_w = ImClamp(ImMax(header_w, body_w), min_w, max_w);
            const float n_h = heights[i];

            const float content_avail = n_w - hdl_w - pad * 2.f;
            const float title_avail = content_avail - tsw - pad * 0.5f - wdot - pad * 0.5f;

            const std::string title_display = truncate_text(n.title.c_str(), title_avail);
            const std::string msg_display = truncate_text(n.message.c_str(), content_avail);

            const float anim_ease = n.closing ? ease_in(1.f - n.anim_t) : ease_out(n.anim_t);
            const float alpha = n.anim_t;
            const float slide = (1.f - anim_ease) * (wm_on_right ? 1.f : -1.f) * 18.f;

            float ny;
            if (wm_on_bottom) {
                float offset = 0.f;
                for (int j = 0; j <= i; j++) offset += heights[j] + gap;
                ny = wm_pos.y - offset;
            }
            else {
                float offset = 0.f;
                for (int j = 0; j < i; j++) offset += heights[j] + gap;
                ny = wm_pos.y + wm_h + gap + offset;
            }

            const float nx = wm_pos.x + slide;

            const ImVec2 box_min = ImVec2(floorf(nx), floorf(ny));
            const ImVec2 box_max = ImVec2(floorf(nx + n_w), floorf(ny + n_h));

            const ImColor tc = type_color(n.type);

            if (var->window.shadow_enabled && var->window.shadow_size > 0.f && var->window.shadow_alpha > 0.f) {
                constexpr int steps = 8;
                for (int s = steps; s >= 1; s--) {
                    const float t = static_cast<float>(s) / steps;
                    const float expand = t * var->window.shadow_size;
                    const float sa = var->window.shadow_alpha * (1.f - t) * alpha;
                    bg->AddRectFilled(
                        box_min - ImVec2(expand, expand),
                        box_max + ImVec2(expand, expand),
                        gc(tc, sa));
                }
            }

            bg->AddRectFilled(box_min, box_max, gc(clr->window.background_one, alpha));

            bg->AddLine(box_min + ImVec2(1.f, 0.f),
                ImVec2(box_max.x - 1.f, box_min.y),
                gc(tc, alpha), 1.f);
            bg->AddLine(box_min + ImVec2(1.f, 1.f),
                ImVec2(box_max.x - 1.f, box_min.y + 1.f),
                gc(tc, alpha * 0.35f), 1.f);

            renderer->rect(bg, box_min, box_max, gc(clr->window.stroke, alpha));

            bg->AddLine(
                ImVec2(box_min.x + hdl_w, box_min.y + 2.f),
                ImVec2(box_min.x + hdl_w, box_min.y + bar_h - 2.f),
                gc(clr->window.stroke, alpha), 1.f);

            const float title_x = box_min.x + hdl_w + pad;
            const float title_y = box_min.y + pad;

            renderer->text_outline(bg, var->font.tahoma, fsz,
                ImVec2(title_x, title_y), gc(tc, alpha), type_str);

            const float dot_x = title_x + tsw + pad * 0.5f;
            renderer->text_outline(bg, var->font.tahoma, fsz,
                ImVec2(dot_x, title_y),
                gc(clr->widgets.text_inactive, alpha * 0.5f), "|");

            renderer->text_outline(bg, var->font.tahoma, fsz,
                ImVec2(dot_x + wdot + pad * 0.5f, title_y),
                gc(clr->widgets.text, alpha), title_display.c_str());

            renderer->text_outline(bg, var->font.tahoma, fsz,
                ImVec2(box_min.x + hdl_w + pad, box_min.y + bar_h + pad),
                gc(clr->widgets.text_inactive, alpha), msg_display.c_str());

            const float progress = 1.f - ImClamp(n.elapsed / n.duration, 0.f, 1.f);
            const float pb_y = box_max.y - prog_h;

            bg->AddRectFilled(
                ImVec2(box_min.x + 1.f, pb_y),
                ImVec2(box_max.x - 1.f, box_max.y - 1.f),
                gc(clr->window.background_two, alpha));

            if (progress > 0.f) {
                bg->AddRectFilled(
                    ImVec2(box_min.x + 1.f, pb_y),
                    ImVec2(box_min.x + 1.f + (n_w - 2.f) * progress, box_max.y - 1.f),
                    gc(tc, alpha * 0.8f));
            }
        }
    }
}