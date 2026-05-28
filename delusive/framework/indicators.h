#pragma once

#include "settings/functions.h"
#include "../configs/configs.h"
#include <Windows.h>
#include <unordered_map>
#include <algorithm>
#include "../scripts/scripts.h"
#include "elements/esp_preview.h"
#include "../game/features/misc/violation/violation.h"
#include "../game/features/esp/shared/shared.h"
#include "elements/notifications.h"
#include "../core/esp_renderer.h"
#include <cmath>

extern bool g_traps_active;

inline void c_gui::render_keybinds_list()
{
	if (config::menu::overlay.value.size() < 2 || !config::menu::overlay.value[1]) 
		return;

	static float window_alpha = 0.f;
	static float placeholder_alpha = 1.f;

	bool has_active_binds = false;
	bool has_animating_binds = false;

	for (auto& b : registered_keybinds)
	{
		int current_mode = (b.mode != nullptr) ? *b.mode : 0;
		bool is_active = false;

		if (current_mode != 2 && (b.key == nullptr || *b.key == 0)) {
			is_active = false;
		}
		else if (b.state != nullptr && !(*b.state)) {
			is_active = false;
		}
		else {
			if (current_mode == 0) is_active = (GetAsyncKeyState(*b.key) & 0x8000);
			else if (current_mode == 1) is_active = (GetKeyState(*b.key) & 0x0001);
			else if (current_mode == 2) is_active = true;
		}

		if (is_active)
			has_active_binds = true;

		b.alpha = ImClamp(b.alpha + (gui->fixed_speed(15.f) * (is_active ? 1.f : -1.f)), 0.f, 1.f);

		if (b.alpha > 0.01f) {
			has_animating_binds = true;
		}
	}

	bool show_placeholder = var->gui.menu_opened && !has_animating_binds;
	placeholder_alpha = ImClamp(placeholder_alpha + (gui->fixed_speed(15.f) * (show_placeholder ? 1.f : -1.f)), 0.f, 1.f);

	const bool show_window = has_active_binds || var->gui.menu_opened;
	window_alpha = ImClamp(window_alpha + (gui->fixed_speed(8.f) * (show_window ? 1.f : -1.f)), 0.f, 1.f);

	if (window_alpha <= 0.01f)
		return;

	auto gc = [&](ImColor color, float a) -> ImU32 {
		const float base_a = (a < 0.f ? color.Value.w : a);
		return draw->get_clr(ImColor(color.Value.x, color.Value.y, color.Value.z, base_a * window_alpha));
		};

	gui->push_style_color(ImGuiCol_WindowBg, draw->get_clr(ImColor(0.f, 0.f, 0.f, 0.f)));
	gui->push_style_color(ImGuiCol_Border, draw->get_clr(ImColor(0.f, 0.f, 0.f, 0.f)));
	gui->push_style_color(ImGuiCol_ScrollbarBg, draw->get_clr(ImColor(0.f, 0.f, 0.f, 0.f)));

	gui->push_style_var(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	gui->push_style_var(ImGuiStyleVar_WindowBorderSize, 0.f);
	gui->push_style_var(ImGuiStyleVar_WindowRounding, 0.f);

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
		ImGuiWindowFlags_NoSavedSettings;

	if (!var->gui.menu_opened || config::menu::lock_layout.value) {
		window_flags |= ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoInputs;
	}

	ImVec2 init_pos = ImVec2(config::menu::keybinds_x.value, config::menu::keybinds_y.value);
	if (init_pos.x < 0.f || init_pos.y < 0.f) {
		init_pos = ImVec2(10.f, 100.f);
	}
	ImGui::SetNextWindowPos(init_pos, ImGuiCond_Appearing);

	if (gui->begin("keybind_list_window", nullptr, window_flags))
	{
		const ImVec2 pos = GetWindowPos();
		if (pos.x != config::menu::keybinds_x.value || pos.y != config::menu::keybinds_y.value) {
			config::menu::keybinds_x.value = pos.x;
			config::menu::keybinds_y.value = pos.y;
		}
		const ImVec2 size = GetWindowSize();
		ImDrawList* draw_list = GetWindowDrawList();

		draw->rect(GetBackgroundDrawList(), pos - ImVec2(1, 1), pos + size + ImVec2(1, 1), gc(ImColor(0.f, 0.f, 0.f, 0.5f), -1.f));
		draw->rect_filled(draw_list, pos, pos + size, gc(clr->window.background_one, -1.f));
		draw->line(draw_list, pos + ImVec2(1, 1), pos + ImVec2(size.x - 1, 1), gc(clr->accent, -1.f), 1);
		draw->line(draw_list, pos + ImVec2(1, 2), pos + ImVec2(size.x - 1, 2), gc(clr->accent, 0.4f), 1);
		draw->rect(draw_list, pos, pos + size, gc(clr->window.stroke, -1.f));

		draw_list->PushClipRect(pos + ImVec2(1, 1), pos + size - ImVec2(1, 1), true);

		const char* title = "keybinds";
		ImVec2 title_size = var->font.tahoma->CalcTextSizeA(var->font.tahoma->FontSize, FLT_MAX, 0.f, title);

		draw->text_outline(draw_list, var->font.tahoma, var->font.tahoma->FontSize,
			pos + ImVec2(std::floor(size.x / 2 - title_size.x / 2), 6.f),
			gc(clr->widgets.text, -1.f), title);

		draw->line(draw_list, pos + ImVec2(6, 22), pos + ImVec2(size.x - 6, 22), gc(clr->window.stroke, -1.f), 1);

		float y_offset = 28.f;

		if (placeholder_alpha > 0.01f)
		{
			const char* placeholder_name = "example bind";
			const char* placeholder_mode = "[hold]";
			ImVec2 mode_size = var->font.tahoma->CalcTextSizeA(var->font.tahoma->FontSize, FLT_MAX, 0.f, placeholder_mode);

			float current_y = std::floor(y_offset);

			draw->text_outline(draw_list, var->font.tahoma, var->font.tahoma->FontSize,
				pos + ImVec2(8.f, current_y),
				gc(clr->widgets.text_inactive, placeholder_alpha), placeholder_name);

			draw->text_outline(draw_list, var->font.tahoma, var->font.tahoma->FontSize,
				pos + ImVec2(size.x - 8.f - mode_size.x, current_y),
				gc(clr->widgets.text_inactive, placeholder_alpha), placeholder_mode);

			y_offset += 16.f * placeholder_alpha;
		}

		for (const auto& bind : registered_keybinds)
		{
			if (bind.alpha <= 0.01f)
				continue;

			int m = (bind.mode) ? *bind.mode : 0;
			const char* mode_str = (m == 0) ? "[hold]" : (m == 1) ? "[toggle]" : "[always]";
			ImVec2 mode_size = var->font.tahoma->CalcTextSizeA(var->font.tahoma->FontSize, FLT_MAX, 0.f, mode_str);

			float slide_x = 10.f * (1.f - bind.alpha);
			float current_y = std::floor(y_offset);

			draw->text_outline(draw_list, var->font.tahoma, var->font.tahoma->FontSize,
				pos + ImVec2(8.f + slide_x, current_y),
				gc(clr->widgets.text, bind.alpha), bind.display_name.c_str());

			draw->text_outline(draw_list, var->font.tahoma, var->font.tahoma->FontSize,
				pos + ImVec2(size.x - 8.f - mode_size.x - slide_x, current_y),
				gc(clr->widgets.text_inactive, bind.alpha), mode_str);

			y_offset += 16.f * bind.alpha;
		}

		draw_list->PopClipRect();

		gui->set_cursor_pos(ImVec2(160.f, y_offset + 4.f));
		ImGui::Dummy(ImVec2(0, 0));
	}
	gui->end();

	gui->pop_style_var(3);
	gui->pop_style_color(3);
}

inline void c_gui::watermark() {

	if (config::menu::overlay.value.size() < 1 || !config::menu::overlay.value[0]) 
		return;

	{
		ImDrawList* bg = GetBackgroundDrawList();
		ImGuiIO& io = GetIO();

		ImVec2 wm_pos = ImVec2(config::menu::watermark_x.value, config::menu::watermark_y.value);
		static bool   wm_dragging = false;
		static ImVec2 wm_drag_off = {};

		static int    wm_order[3] = { 0, 1, 2 };
		static int    wm_elem_from = -1;
		static int    wm_elem_to = -1;
		static bool   wm_elem_dragging = false;
		static ImVec2 wm_elem_drag_start = {};

		const int   fps = static_cast<int>(io.Framerate);
		const float fsz = var->font.tahoma->FontSize;
		const float pad = 6.f;
		const float sep = 6.f;
		const float hdl_w = 14.f;

		auto gc = [&](ImColor color, float alpha) -> ImU32 {
			const float a = (alpha < 0.f ? color.Value.w : alpha);
			return draw->get_clr(color, a);
			};

		const char* seg0 = "delusive.pro";

		std::string username = g_discord_username.empty() ? "connecting..." : g_discord_username;

		char seg1[64], seg2[32];
		ImFormatString(seg1, sizeof(seg1), "%s", username.c_str());
		ImFormatString(seg2, sizeof(seg2), "%d fps", fps);
		const char* dot = "|";

		const char* segs[3] = { seg0, seg1, seg2 };

		auto seg_color = [&](int i) -> ImU32 {
			if (i == 0) return gc(clr->accent, -1.f);
			if (i == 1) return gc(clr->widgets.text, -1.f);
			return gc(clr->widgets.text_inactive, -1.f);
			};

		auto seg_color_base = [&](int i) -> ImColor {
			if (i == 0) return clr->accent;
			if (i == 1) return clr->widgets.text;
			return clr->widgets.text_inactive;
			};

		const float av_sz = fsz;
		const float av_gap = 4.f;
		const float av_w = g_discord_avatar ? (av_sz + av_gap) : 0.f;

		float sw[3];
		for (int i = 0; i < 3; i++)
			sw[i] = var->font.tahoma->CalcTextSizeA(fsz, FLT_MAX, 0.f, segs[i]).x;
		sw[1] += av_w;

		const float wdot = var->font.tahoma->CalcTextSizeA(fsz, FLT_MAX, 0.f, dot).x;

		const float total_w = hdl_w + pad
			+ sw[0] + sep + wdot + sep
			+ sw[1] + sep + wdot + sep
			+ sw[2] + pad;
		const float total_h = fsz + pad * 2.f;

		if (wm_pos.x < 0.f || wm_pos.y < 0.f)
			wm_pos = ImVec2(10.f, 10.f);

		const ImVec2 box_min = wm_pos;
		const ImVec2 box_max = wm_pos + ImVec2(total_w, total_h);
		const float  cy = box_min.y + pad;

		const bool is_interactive = var->gui.menu_opened;

		const bool hovered = is_interactive && io.MousePos.x >= box_min.x && io.MousePos.x <= box_max.x
			&& io.MousePos.y >= box_min.y && io.MousePos.y <= box_max.y;
		const bool hdl_hov = hovered && io.MousePos.x <= box_min.x + hdl_w;

		if (hdl_hov && io.MouseClicked[0] && !wm_elem_dragging
			&& !config::menu::lock_layout.value)         
		{
			wm_dragging = true;
			wm_drag_off = io.MousePos - wm_pos;
		}
		if (!io.MouseDown[0]) wm_dragging = false;
		if (wm_dragging)
			wm_pos = io.MousePos - wm_drag_off;

		wm_pos.x = ImClamp(wm_pos.x, 0.f, io.DisplaySize.x - total_w);
		wm_pos.y = ImClamp(wm_pos.y, 0.f, io.DisplaySize.y - total_h);

		if (config::menu::watermark_x.value != wm_pos.x || config::menu::watermark_y.value != wm_pos.y) {
			config::menu::watermark_x.value = wm_pos.x;
			config::menu::watermark_y.value = wm_pos.y;
		}

		float slot_x[3];
		{
			float x = box_min.x + hdl_w + pad;
			for (int s = 0; s < 3; s++)
			{
				slot_x[s] = x;
				x += sw[wm_order[s]];
				if (s < 2) x += sep + wdot + sep;
			}
		}

		const float content_x_min = box_min.x + hdl_w + pad;
		const float content_x_max = box_max.x - pad;

		int hov_slot = -1;

		if (!wm_dragging)
		{
			if (wm_elem_dragging)
			{
				const float mid01 = (slot_x[0] + sw[wm_order[0]] + slot_x[1]) * 0.5f;
				const float mid12 = (slot_x[1] + sw[wm_order[1]] + slot_x[2]) * 0.5f;

				if (io.MousePos.x < mid01) hov_slot = 0;
				else if (io.MousePos.x < mid12) hov_slot = 1;
				else                            hov_slot = 2;
			}
			else if (hovered && !hdl_hov)
			{
				for (int s = 0; s < 3; s++)
				{
					const float x0 = slot_x[s] - (s > 0 ? sep * 0.5f : 0.f);
					const float x1 = slot_x[s] + sw[wm_order[s]] + (s < 2 ? sep * 0.5f : 0.f);
					if (io.MousePos.x >= x0 && io.MousePos.x <= x1) { hov_slot = s; break; }
				}
			}
		}

		if (hov_slot >= 0 && io.MouseClicked[0] && !wm_dragging && !wm_elem_dragging
			&& !config::menu::lock_layout.value)
		{
			wm_elem_from = hov_slot;
			wm_elem_to = hov_slot;
			wm_elem_drag_start = io.MousePos;
		}

		if (!wm_elem_dragging && wm_elem_from >= 0 && io.MouseDown[0])
			if (ImLengthSqr(io.MousePos - wm_elem_drag_start) > 16.f)
				wm_elem_dragging = true;

		if (wm_elem_dragging && hov_slot >= 0)
			wm_elem_to = hov_slot;

		if (!io.MouseDown[0])
		{
			if (wm_elem_dragging && wm_elem_from >= 0 && wm_elem_to >= 0 && wm_elem_from != wm_elem_to)
				std::swap(wm_order[wm_elem_from], wm_order[wm_elem_to]);
			wm_elem_from = wm_elem_to = -1;
			wm_elem_dragging = false;
		}

		if (var->window.shadow_enabled && var->window.shadow_size > 0.f && var->window.shadow_alpha > 0.f)
		{
			draw->shadow_rect(bg, box_min, box_max,
				gc(clr->accent, var->window.shadow_alpha),
				var->window.shadow_size,
				ImVec2(0.f, 0.f),
				ImDrawFlags_ShadowCutOutShapeBackground,
				0.f);
		}

		draw->rect_filled(bg, box_min, box_max, gc(clr->window.background_one, -1.f));

		draw->line(bg,
			box_min + ImVec2(1.f, 1.f), ImVec2(box_max.x - 1.f, box_min.y + 1.f),
			gc(clr->accent, -1.f), 1.f);
		draw->line(bg,
			box_min + ImVec2(1.f, 2.f), ImVec2(box_max.x - 1.f, box_min.y + 2.f),
			gc(clr->accent, 0.4f), 1.f);


		draw->rect(bg, box_min, box_max,
			gc((hovered || wm_dragging) ? clr->accent : clr->window.stroke, -1.f));

		{
			const ImU32 dot_clr = gc(clr->accent, (hdl_hov || wm_dragging) ? 0.9f : 0.35f);
			const float r = 1.1f;
			const float cx_dot = box_min.x + hdl_w * 0.5f - r;
			const float cy_mid = box_min.y + total_h * 0.5f;
			for (int col = 0; col < 2; col++)
				for (int row = 0; row < 3; row++)
				{
					bg->AddCircleFilled(
						ImVec2(cx_dot + col * (r * 2.f + 1.5f),
							cy_mid + (row - 1) * (r * 2.f + 1.5f)),
						r, dot_clr, 6);
				}
		}

		bg->AddLine(
			ImVec2(box_min.x + hdl_w, box_min.y + 2.f),
			ImVec2(box_min.x + hdl_w, box_max.y - 2.f),
			gc(clr->window.stroke, -1.f), 1.f);

		float draw_cx = box_min.x + hdl_w + pad;

		for (int s = 0; s < 3; s++)
		{
			const int   elem = wm_order[s];
			const float w = sw[elem];
			const bool  is_from = wm_elem_dragging && wm_elem_from == s;
			const bool  is_to = wm_elem_dragging && wm_elem_to == s && !is_from;

			if (is_to)
				bg->AddRectFilled(
					ImVec2(draw_cx - sep * 0.5f, box_min.y + 1.f),
					ImVec2(draw_cx + w + sep * 0.5f, box_max.y - 1.f),
					gc(clr->accent, 0.15f));

			if (hov_slot == s && !wm_elem_dragging)
				bg->AddRectFilled(
					ImVec2(draw_cx - 2.f, box_min.y + 1.f),
					ImVec2(draw_cx + w + 2.f, box_max.y - 1.f),
					gc(clr->widgets.text, 0.05f));

			ImU32 color = seg_color(elem);
			if (is_from)
			{
				ImVec4 cv = ImGui::ColorConvertU32ToFloat4(color);
				cv.w *= 0.35f;
				color = ImGui::ColorConvertFloat4ToU32(cv);
			}

			float text_draw_x = draw_cx;
			if (elem == 1 && g_discord_avatar)
			{
				ImVec2 av_min = ImVec2(draw_cx, cy);
				ImVec2 av_max = ImVec2(draw_cx + av_sz, cy + av_sz);
				bg->AddImageRounded((ImTextureID)g_discord_avatar, av_min, av_max, ImVec2(0, 1), ImVec2(1, 0), color, 2.f);
				text_draw_x += av_sz + av_gap;
			}

			draw->text_outline(bg, var->font.tahoma, fsz, ImVec2(text_draw_x, cy), color, segs[elem]);
			draw_cx += w;

			if (s < 2)
			{
				draw_cx += sep;
				draw->text_outline(bg, var->font.tahoma, fsz, ImVec2(draw_cx, cy),
					gc(clr->widgets.text_inactive, -1.f), dot);
				draw_cx += wdot + sep;
			}
		}

		if (wm_elem_dragging && wm_elem_from >= 0)
		{
			const int   elem = wm_order[wm_elem_from];
			const float ghost_x = ImClamp(
				io.MousePos.x - sw[elem] * 0.5f,
				content_x_min,
				content_x_max - sw[elem]);

			float ghost_text_x = ghost_x;
			ImU32 ghost_color = gc(seg_color_base(elem), 0.85f);
			if (elem == 1 && g_discord_avatar)
			{
				ImVec2 av_min = ImVec2(ghost_x, cy);
				ImVec2 av_max = ImVec2(ghost_x + av_sz, cy + av_sz);
				bg->AddImageRounded((ImTextureID)g_discord_avatar, av_min, av_max, ImVec2(0, 1), ImVec2(1, 0), ghost_color, 2.f);
				ghost_text_x += av_sz + av_gap;
			}

			bg->AddText(var->font.tahoma, fsz,
				ImVec2(ghost_text_x, cy),
				ghost_color,
				segs[elem]);
		}

		notifications::draw(box_min, total_w, total_h);
	}
}

inline void c_gui::render_reload_indicator()
{
	if (config::menu::overlay.value.size() < 3 || !config::menu::overlay.value[2]) 
		return;

	static float window_alpha = 0.f;

	bool is_menu_open = var->gui.menu_opened;
	bool is_reloading = g_is_reloading;
	float progress = g_reload_progress;
	bool is_on_server = g_is_on_server;

	if (is_menu_open && !is_reloading) {
		is_reloading = true;
		progress = std::fmodf((float)ImGui::GetTime() * 0.7f, 1.0f);
	}

	const bool show_window = is_reloading && (is_on_server || is_menu_open);
	window_alpha = ImClamp(window_alpha + (gui->fixed_speed(8.f) * (show_window ? 1.f : -1.f)), 0.f, 1.f);

	if (window_alpha <= 0.01f)
		return;

	auto gc = [&](ImColor color, float a) -> ImU32 {
		const float base_a = (a < 0.f ? color.Value.w : a);
		return draw->get_clr(ImColor(color.Value.x, color.Value.y, color.Value.z, base_a * window_alpha));
		};

	gui->push_style_color(ImGuiCol_WindowBg, draw->get_clr(ImColor(0.f, 0.f, 0.f, 0.f)));
	gui->push_style_color(ImGuiCol_Border, draw->get_clr(ImColor(0.f, 0.f, 0.f, 0.f)));
	gui->push_style_color(ImGuiCol_ScrollbarBg, draw->get_clr(ImColor(0.f, 0.f, 0.f, 0.f)));

	gui->push_style_var(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	gui->push_style_var(ImGuiStyleVar_WindowBorderSize, 0.f);
	gui->push_style_var(ImGuiStyleVar_WindowRounding, 0.f);

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
		ImGuiWindowFlags_NoSavedSettings;

	if (!var->gui.menu_opened || config::menu::lock_layout.value) {
		window_flags |= ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoInputs;
	}

	const float W = 150.f;
	const float H = 38.f;
	ImGui::SetNextWindowSize(ImVec2(W, H), ImGuiCond_Always);

	ImVec2 init_pos = ImVec2(config::menu::reload_x.value, config::menu::reload_y.value);
	if (init_pos.x < 0.f || init_pos.y < 0.f) {
		init_pos = ImVec2(10.f, 200.f);
	}
	ImGui::SetNextWindowPos(init_pos, ImGuiCond_Appearing);

	if (gui->begin("reload_indicator_window", nullptr, window_flags))
	{
		const ImVec2 pos = GetWindowPos();
		if (pos.x != config::menu::reload_x.value || pos.y != config::menu::reload_y.value) {
			config::menu::reload_x.value = pos.x;
			config::menu::reload_y.value = pos.y;
		}
		const ImVec2 size = GetWindowSize();
		ImDrawList* draw_list = GetWindowDrawList();

		draw->rect(GetBackgroundDrawList(), pos - ImVec2(1, 1), pos + size + ImVec2(1, 1), gc(ImColor(0.f, 0.f, 0.f, 0.5f), -1.f));
		draw->rect_filled(draw_list, pos, pos + size, gc(clr->window.background_one, -1.f));
		draw->line(draw_list, pos + ImVec2(1, 1), pos + ImVec2(size.x - 1, 1), gc(clr->accent, -1.f), 1);
		draw->line(draw_list, pos + ImVec2(1, 2), pos + ImVec2(size.x - 1, 2), gc(clr->accent, 0.4f), 1);
		draw->rect(draw_list, pos, pos + size, gc(clr->window.stroke, -1.f));

		draw_list->PushClipRect(pos + ImVec2(1, 1), pos + size - ImVec2(1, 1), true);

		char text_buf[64];
		sprintf_s(text_buf, sizeof(text_buf), "reloading [%d%%]", (int)(progress * 100.f));
		ImVec2 text_size = var->font.tahoma->CalcTextSizeA(var->font.tahoma->FontSize, FLT_MAX, 0.f, text_buf);

		draw->text_outline(draw_list, var->font.tahoma, var->font.tahoma->FontSize,
			pos + ImVec2(std::floor(size.x / 2 - text_size.x / 2), 6.f),
			gc(clr->widgets.text, -1.f), text_buf);

		float bar_pad_x = 10.f;
		float bar_h = 3.f;
		float bar_y = size.y - 8.f;

		ImU32 bar_bg = gc(clr->window.stroke, -1.f);
		ImU32 bar_fill = gc(clr->accent, -1.f);

		draw_list->AddRectFilled(pos + ImVec2(bar_pad_x, bar_y), pos + ImVec2(size.x - bar_pad_x, bar_y + bar_h), bar_bg, 1.5f);

		if (progress > 0.01f) {
			draw_list->AddRectFilled(pos + ImVec2(bar_pad_x, bar_y), pos + ImVec2(bar_pad_x + (size.x - bar_pad_x * 2.f) * progress, bar_y + bar_h), bar_fill, 1.5f);
		}

		draw_list->PopClipRect();
	}
	gui->end();

	gui->pop_style_color(3);
	gui->pop_style_var(3);
}

inline void c_gui::render_flyhack_indicator()
{
	bool is_menu_open = var->gui.menu_opened;
	bool is_indicator_enabled = config::menu::overlay.value.size() > 3 && config::menu::overlay.value[3];
	bool is_on_server = g_is_on_server;

	float progress = 0.0f;
	if (is_on_server) {
		progress = std::clamp(violation::g_indicator_pct / 100.f, 0.f, 1.f);
	}

	if (is_menu_open && progress <= 0.01f) {
		progress = std::fmodf((float)ImGui::GetTime() * 0.7f, 1.0f);
	}

	bool show_window = is_indicator_enabled && (is_menu_open || is_on_server);

	static float alpha = 0.f;
	alpha = ImClamp(alpha + (gui->fixed_speed(8.f) * (show_window ? 1.f : -1.f)), 0.f, 1.f);

	if (alpha <= 0.01f) return;

	static float smooth_progress = 0.0f;
	float dt = ImGui::GetIO().DeltaTime;
	float lerp_speed = (progress < smooth_progress) ? 12.0f : 4.0f;
	smooth_progress = ImLerp(smooth_progress, progress, ImClamp(dt * lerp_speed, 0.f, 1.f));

	static bool init_pos = false;
	static ImVec2 pos = { 0, 0 };
	if (!init_pos) {
		pos = { (float)config::visuals::interfaces::anti_fly_hack_kick_indi_X.value, (float)config::visuals::interfaces::anti_fly_hack_kick_indi_Y.value };
		if (pos.x <= 0.f || pos.y <= 0.f) {
			pos = { 10.f, 300.f };
		}
		init_pos = true;
	}

	const float W = 150.f;
	const float H = 44.f;  

	ImGuiIO& io = ImGui::GetIO();
	ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(W, H));

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoNav;

	if (!is_menu_open) flags |= ImGuiWindowFlags_NoInputs;

	gui->push_style_color(ImGuiCol_WindowBg, draw->get_clr(ImColor(0.f, 0.f, 0.f, 0.f)));
	gui->push_style_color(ImGuiCol_Border, draw->get_clr(ImColor(0.f, 0.f, 0.f, 0.f)));

	if (ImGui::Begin(_("##flyhack_indicator_wnd"), nullptr, flags)) {
		if (is_menu_open) {
			ImGui::SetCursorScreenPos(pos);
			ImGui::InvisibleButton(_("##flyhack_drag"), ImVec2(W, H));
			if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0)) {
				pos.x += io.MouseDelta.x;
				pos.y += io.MouseDelta.y;
				config::visuals::interfaces::anti_fly_hack_kick_indi_X.value = pos.x;
				config::visuals::interfaces::anti_fly_hack_kick_indi_Y.value = pos.y;
			}
		}
	}
	ImGui::End();

	gui->pop_style_color(2);

	auto C = [&](ImColor col, float mult = 1.f) -> ImU32 {
		return draw->get_clr(ImColor(col.Value.x, col.Value.y, col.Value.z,
			col.Value.w * alpha * mult));
	};

	auto bar_color = [&](float t, float a_mult = 1.f) -> ImU32 {
		float rv, gv, bv;
		if (t < 0.5f) {
			rv = t * 2.f; gv = 1.f; bv = 0.f;
		} else if (t < 0.8f) {
			float sv = (t - 0.5f) / 0.3f;
			rv = 1.f; gv = 1.f - sv * 0.5f; bv = 0.f;
		} else {
			float sv = (t - 0.8f) / 0.2f;
			rv = 1.f; gv = (0.5f - sv * 0.5f > 0.f) ? (0.5f - sv * 0.5f) : 0.f; bv = 0.f;
		}
		return draw->get_clr(ImColor(rv, gv, bv, alpha * a_mult));
	};

	ImDrawList* dl = ImGui::GetBackgroundDrawList();
	const ImVec2 p = pos;
	const ImVec2 m = { p.x + W, p.y + H };

	draw->rect_filled(dl, p, m, C(clr->window.background_one, 1.f));
	draw->rect(dl, p, m, C(clr->window.stroke, 1.f));

	ImU32 dyn_col      = bar_color(smooth_progress, 1.f);
	ImU32 dyn_col_fade = bar_color(smooth_progress, 0.35f);
	draw->line(dl, p + ImVec2(1, 1), p + ImVec2(W - 1, 1), dyn_col,      1);
	draw->line(dl, p + ImVec2(1, 2), p + ImVec2(W - 1, 2), dyn_col_fade, 1);

	char text_buf[64];
	sprintf_s(text_buf, sizeof(text_buf), "fly violation [%d%%]", (int)(smooth_progress * 100.f));
	ImVec2 text_size = var->font.tahoma->CalcTextSizeA(var->font.tahoma->FontSize, FLT_MAX, 0.f, text_buf);
	draw->text_outline(dl, var->font.tahoma, var->font.tahoma->FontSize,
		ImVec2(p.x + (W - text_size.x) * 0.5f, p.y + 5.f),
		C(clr->widgets.text, 1.f), text_buf);

	const char* status_str;
	ImU32 status_col;
	if (smooth_progress < 0.3f) {
		status_str = "safe";
		status_col = bar_color(0.05f, 1.f);
	} else if (smooth_progress < 0.6f) {
		status_str = "warning";
		status_col = bar_color(0.45f, 1.f);
	} else if (smooth_progress < 0.82f) {
		status_str = "danger";
		status_col = bar_color(0.72f, 1.f);
	} else {
		float pulse = (std::sin((float)ImGui::GetTime() * 10.f) + 1.f) * 0.5f;
		status_col = bar_color(1.f, 0.55f + pulse * 0.45f);
		status_str = "critical!";
	}
	ImVec2 status_size = var->font.tahoma->CalcTextSizeA(var->font.tahoma->FontSize, FLT_MAX, 0.f, status_str);
	draw->text_outline(dl, var->font.tahoma, var->font.tahoma->FontSize,
		ImVec2(p.x + (W - status_size.x) * 0.5f,
			   p.y + 5.f + var->font.tahoma->FontSize + 3.f),
		status_col, status_str);

	const float bar_pad_x = 8.f;
	const float bar_h     = 3.f;
	const float bar_y     = m.y - 8.f;

	ImU32 bar_bg   = C(clr->window.stroke, 1.f);
	ImU32 bar_fill = bar_color(smooth_progress, 1.f);

	if (smooth_progress > 0.8f) {
		float pulse = (std::sin((float)ImGui::GetTime() * 15.0f) + 1.0f) * 0.5f;
		bar_fill = bar_color(smooth_progress, 0.5f + pulse * 0.5f);
	}

	dl->AddRectFilled(
		ImVec2(p.x + bar_pad_x, bar_y),
		ImVec2(m.x - bar_pad_x, bar_y + bar_h),
		bar_bg, 1.5f);

	if (smooth_progress > 0.01f) {
		dl->AddRectFilled(
			ImVec2(p.x + bar_pad_x, bar_y),
			ImVec2(p.x + bar_pad_x + (W - bar_pad_x * 2.f) * smooth_progress, bar_y + bar_h),
			bar_fill, 1.5f);
	}
}

inline void c_gui::render_traps_indicator()
{
	bool is_menu_open = var->gui.menu_opened;
	bool is_indicator_enabled = config::menu::overlay.value.size() > 4 && config::menu::overlay.value[4];
	bool is_on_server = g_is_on_server;

	float target_progress = (is_on_server && g_traps_active) ? 1.0f : 0.0f;

	if (is_menu_open && target_progress <= 0.01f) {
		target_progress = 1.0f;
	}

	bool show_window = is_indicator_enabled && (is_menu_open || (is_on_server && target_progress > 0.01f));

	static float alpha = 0.f;
	alpha = ImClamp(alpha + (gui->fixed_speed(8.f) * (show_window ? 1.f : -1.f)), 0.f, 1.f);

	if (alpha <= 0.01f) return;

	static float smooth_progress = 0.0f;
	float dt = ImGui::GetIO().DeltaTime;
	smooth_progress = ImLerp(smooth_progress, target_progress, ImClamp(dt * 10.0f, 0.f, 1.f));

	static bool init_pos = false;
	static ImVec2 pos = { 0, 0 };
	if (!init_pos) {
		pos = { (float)config::visuals::interfaces::traps_indicator_X.value, (float)config::visuals::interfaces::traps_indicator_Y.value };
		if (pos.x <= 0.f || pos.y <= 0.f) {
			pos = { 10.f, 360.f };
		}
		init_pos = true;
	}

	const float W = 150.f;
	const float H = 44.f;

	ImGuiIO& io = ImGui::GetIO();
	ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(W, H));

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoNav;

	if (!is_menu_open) flags |= ImGuiWindowFlags_NoInputs;

	gui->push_style_color(ImGuiCol_WindowBg, draw->get_clr(ImColor(0.f, 0.f, 0.f, 0.f)));
	gui->push_style_color(ImGuiCol_Border, draw->get_clr(ImColor(0.f, 0.f, 0.f, 0.f)));

	if (ImGui::Begin(_("##traps_indicator_wnd"), nullptr, flags)) {
		if (is_menu_open) {
			ImGui::SetCursorScreenPos(pos);
			ImGui::InvisibleButton(_("##traps_drag"), ImVec2(W, H));
			if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0)) {
				pos.x += io.MouseDelta.x;
				pos.y += io.MouseDelta.y;
				config::visuals::interfaces::traps_indicator_X.value = pos.x;
				config::visuals::interfaces::traps_indicator_Y.value = pos.y;
			}
		}
	}
	ImGui::End();

	gui->pop_style_color(2);

	auto C = [&](ImColor col, float mult = 1.f) -> ImU32 {
		return draw->get_clr(ImColor(col.Value.x, col.Value.y, col.Value.z,
			col.Value.w * alpha * mult));
	};

	auto bar_color = [&](float t, float a_mult) -> ImU32 {
		return draw->get_clr(ImColor(1.0f - t, t, 0.0f, alpha * a_mult));
	};

	ImDrawList* dl = ImGui::GetBackgroundDrawList();
	const ImVec2 p = pos;
	const ImVec2 m = { p.x + W, p.y + H };

	draw->rect_filled(dl, p, m, C(clr->window.background_one, 1.f));
	draw->rect(dl, p, m, C(clr->window.stroke, 1.f));

	ImU32 dyn_col      = bar_color(smooth_progress, 1.f);
	ImU32 dyn_col_fade = bar_color(smooth_progress, 0.35f);
	draw->line(dl, p + ImVec2(1, 1), p + ImVec2(W - 1, 1), dyn_col,      1);
	draw->line(dl, p + ImVec2(1, 2), p + ImVec2(W - 1, 2), dyn_col_fade, 1);

	char text_buf[64] = "traps exploit";
	ImVec2 text_size = var->font.tahoma->CalcTextSizeA(var->font.tahoma->FontSize, FLT_MAX, 0.f, text_buf);
	draw->text_outline(dl, var->font.tahoma, var->font.tahoma->FontSize,
		ImVec2(p.x + (W - text_size.x) * 0.5f, p.y + 5.f),
		C(clr->widgets.text, 1.f), text_buf);

	const char* status_str = "inactive";
	ImU32 status_col = bar_color(0.0f, 1.f);

	if (smooth_progress > 0.5f) {
		status_str = "active";
		status_col = bar_color(1.0f, 1.f);
	}

	ImVec2 status_size = var->font.tahoma->CalcTextSizeA(var->font.tahoma->FontSize, FLT_MAX, 0.f, status_str);
	draw->text_outline(dl, var->font.tahoma, var->font.tahoma->FontSize,
		ImVec2(p.x + (W - status_size.x) * 0.5f,
			   p.y + 5.f + var->font.tahoma->FontSize + 3.f),
		status_col, status_str);

	const float bar_pad_x = 8.f;
	const float bar_h     = 3.f;
	const float bar_y     = m.y - 8.f;

	ImU32 bar_bg   = C(clr->window.stroke, 1.f);
	ImU32 bar_fill = bar_color(smooth_progress, 1.f);

	dl->AddRectFilled(
		ImVec2(p.x + bar_pad_x, bar_y),
		ImVec2(m.x - bar_pad_x, bar_y + bar_h),
		bar_bg, 1.5f);

	if (smooth_progress > 0.01f) {
		dl->AddRectFilled(
			ImVec2(p.x + bar_pad_x, bar_y),
			ImVec2(p.x + bar_pad_x + (W - bar_pad_x * 2.f) * smooth_progress, bar_y + bar_h),
			bar_fill, 1.5f);
	}
}

inline void c_gui::render_fov()
{
	static float fov_alpha = 0.f;
	bool show_fov = (config::aimbot::legit_enabled.value || config::aimbot::rage_enabled.value) && config::aimbot::draw_fov.value;
	
	fov_alpha = ImClamp(fov_alpha + (gui->fixed_speed(8.f) * (show_fov ? 1.f : -1.f)), 0.f, 1.f);
	
	if (fov_alpha <= 0.01f) {
		return;
	}

	ImGuiIO& io = ImGui::GetIO();
	float cx = io.DisplaySize.x * 0.5f;
	float cy = io.DisplaySize.y * 0.5f;

	bool is_aiming = false;
	if (sdk::local_player && memory::is_valid(sdk::local_player)) {
		auto* ms = sdk::local_player->model_state();
		if (memory::is_valid(ms)) {
			is_aiming = ms->has_flag(enums::e_model_state_flags::in_aim);
		}
	}

	float base_fov    = config::aimbot::fov.value * 1.1f;
	float current_fov = (config::aimbot::dynamic_fov.value && is_aiming)
		? base_fov * 1.4f : base_fov;

	constexpr float DEG_TO_RAD_HALF = 0.00872664625f;
	constexpr float BASE = 90.f;
	float target_radius = (std::tanf(current_fov * DEG_TO_RAD_HALF) / std::tanf(BASE * DEG_TO_RAD_HALF)) * cy;

	static float smooth_radius = -1.f;
	if (smooth_radius < 0.f) {
		smooth_radius = target_radius;
	} else {
		float dt = io.DeltaTime;
		smooth_radius = ImLerp(smooth_radius, target_radius, ImClamp(dt * 12.f, 0.f, 1.f));
	}

	ImColor fov_color = (ImColor)config::aimbot::fov_color.value;
	fov_color.Value.w *= fov_alpha;

	if (smooth_radius > 0.f) {
		esp_render.circle(cx, cy, smooth_radius, fov_color, 1.0f, 64);
	}
}
