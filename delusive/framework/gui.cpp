#include "settings/functions.h"
#include "../configs/configs.h"
#include <Windows.h>
#include <unordered_map>
#include <algorithm>
#include "../scripts/scripts.h"
#include "elements/esp_preview.h"
#include "../game/features/misc/violation.h"

namespace core {
    extern volatile bool g_unloading;
}

void c_gui::render_keybinds_list()
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

	// Фейковый бинд виден ТОЛЬКО когда открыто меню И нет активных биндов
	bool show_placeholder = var->gui.menu_opened && !has_animating_binds;
	placeholder_alpha = ImClamp(placeholder_alpha + (gui->fixed_speed(15.f) * (show_placeholder ? 1.f : -1.f)), 0.f, 1.f);

	const bool show_window = has_active_binds || var->gui.menu_opened;
	window_alpha = ImClamp(window_alpha + (gui->fixed_speed(8.f) * (show_window ? 1.f : -1.f)), 0.f, 1.f);

	if (window_alpha <= 0.01f)
		return;

	auto gc = [&](ImColor color, float a = -1.f) -> ImU32 {
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

		draw->rect(GetBackgroundDrawList(), pos - ImVec2(1, 1), pos + size + ImVec2(1, 1), gc(ImColor(0.f, 0.f, 0.f, 0.5f)));
		draw->rect_filled(draw_list, pos, pos + size, gc(clr->window.background_one));
		draw->line(draw_list, pos + ImVec2(1, 1), pos + ImVec2(size.x - 1, 1), gc(clr->accent), 1);
		draw->line(draw_list, pos + ImVec2(1, 2), pos + ImVec2(size.x - 1, 2), gc(clr->accent, 0.4f), 1);
		draw->rect(draw_list, pos, pos + size, gc(clr->window.stroke));

		draw_list->PushClipRect(pos + ImVec2(1, 1), pos + size - ImVec2(1, 1), true);

		const char* title = "keybinds";
		ImVec2 title_size = var->font.tahoma->CalcTextSizeA(var->font.tahoma->FontSize, FLT_MAX, 0.f, title);

		draw->text_outline(draw_list, var->font.tahoma, var->font.tahoma->FontSize,
			pos + ImVec2(std::floor(size.x / 2 - title_size.x / 2), 6.f),
			gc(clr->widgets.text), title);

		draw->line(draw_list, pos + ImVec2(6, 22), pos + ImVec2(size.x - 6, 22), gc(clr->window.stroke), 1);

		float y_offset = 28.f;

		// --- ФЕЙКОВЫЙ БИНД ---
		if (placeholder_alpha > 0.01f)
		{
			const char* placeholder_name = "example bind";
			const char* placeholder_mode = "[hold]";
			ImVec2 mode_size = var->font.tahoma->CalcTextSizeA(var->font.tahoma->FontSize, FLT_MAX, 0.f, placeholder_mode);

			float current_y = std::floor(y_offset);

			// Делаем текст неактивного цвета (text_inactive), чтобы было понятно, что это пример
			draw->text_outline(draw_list, var->font.tahoma, var->font.tahoma->FontSize,
				pos + ImVec2(8.f, current_y),
				gc(clr->widgets.text_inactive, placeholder_alpha), placeholder_name);

			draw->text_outline(draw_list, var->font.tahoma, var->font.tahoma->FontSize,
				pos + ImVec2(size.x - 8.f - mode_size.x, current_y),
				gc(clr->widgets.text_inactive, placeholder_alpha), placeholder_mode);

			y_offset += 16.f * placeholder_alpha;
		}

		// --- РЕАЛЬНЫЕ БИНДЫ ---
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

void c_gui::watermark() {

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

		auto gc = [&](ImColor color, float alpha = -1.f) -> ImU32 {
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
			if (i == 0) return gc(clr->accent);
			if (i == 1) return gc(clr->widgets.text);
			return gc(clr->widgets.text_inactive);
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

		draw->rect_filled(bg, box_min, box_max, gc(clr->window.background_one));

		draw->line(bg,
			box_min + ImVec2(1.f, 1.f), ImVec2(box_max.x - 1.f, box_min.y + 1.f),
			gc(clr->accent), 1.f);
		draw->line(bg,
			box_min + ImVec2(1.f, 2.f), ImVec2(box_max.x - 1.f, box_min.y + 2.f),
			gc(clr->accent, 0.4f), 1.f);


		draw->rect(bg, box_min, box_max,
			gc((hovered || wm_dragging) ? clr->accent : clr->window.stroke));

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
			gc(clr->window.stroke), 1.f);

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
					gc(clr->widgets.text_inactive), dot);
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

bool c_gui::render_intro()
{
	static enum class State { Typing, Pause, Collapse, Slide, FadeOut, Done } s = State::Typing;
	static float t = 0.f;
	static float saved_d_shift = 0.f;

	if (s == State::Done)
		return false;

	t += ImGui::GetIO().DeltaTime;

	constexpr const char* TEXT = "DELUSIVE.PRO";
	constexpr int         N = 12;

	ImDrawList* fg = ImGui::GetForegroundDrawList();
	const ImVec2 screen = ImGui::GetIO().DisplaySize;

	ImFont* font = var->font.segoe_ui;     
	const float FS = font->FontSize;          
	const float LS = screen.x / 480.f;

	float cw[N];
	float total_w = 0.f;
	for (int i = 0; i < N; i++) {
		char buf[2] = { TEXT[i], 0 };
		cw[i] = font->CalcTextSizeA(FS, FLT_MAX, 0.f, buf).x + (i < N - 1 ? LS : 0.f);
		total_w += cw[i];
	}

	const float base_x = screen.x * 0.5f - total_w * 0.5f;
	const float base_y = (screen.y - FS) * 0.5f;

	constexpr float CHAR_DELAY = 0.07f;
	constexpr float CHAR_FADE = 0.18f;
	constexpr float PAUSE_DUR = 2.00f;
	constexpr float COL_DELAY = 0.04f;
	constexpr float COL_FADE = 0.18f;
	constexpr float SLIDE_DUR = 0.55f;
	constexpr float FADEOUT_DUR = 0.65f;

	float alpha[N] = {};
	float yoff[N] = {};
	float d_shift = 0.f;
	float fade_mul = 1.f;

	switch (s)
	{
	case State::Typing:
		for (int i = 0; i < N; i++) {
			float p = ImClamp((t - i * CHAR_DELAY) / CHAR_FADE, 0.f, 1.f);
			alpha[i] = p;
			yoff[i] = (1.f - p) * 10.f;
		}
		if (t > (N - 1) * CHAR_DELAY + CHAR_FADE + 0.1f) { s = State::Pause; t = 0.f; }
		break;

	case State::Pause:
		for (int i = 0; i < N; i++) alpha[i] = 1.f;
		if (t > PAUSE_DUR) {
			if (g_discord_ready) {
				s = State::Collapse;
				t = 0.f;
			} else {
				t = PAUSE_DUR;
			}
		}
		break;

	case State::Collapse:
		alpha[0] = 1.f;
		for (int i = 1; i < N; i++) {
			float p = ImClamp((t - (i - 1) * COL_DELAY) / COL_FADE, 0.f, 1.f);
			alpha[i] = 1.f - p;
			yoff[i] = -p * 8.f;
		}
		if (t > (N - 2) * COL_DELAY + COL_FADE + 0.1f) {
			saved_d_shift = 0.f;
			s = State::Slide; t = 0.f;
		}
		break;

	case State::Slide: {
		alpha[0] = 1.f;
		float p = ImClamp(t / SLIDE_DUR, 0.f, 1.f);
		p = p * p * (3.f - 2.f * p);
		float d_target = screen.x * 0.5f - cw[0] * 0.5f;
		d_shift = (d_target - base_x) * p;
		saved_d_shift = d_shift;
		if (t > SLIDE_DUR + 0.15f) { s = State::FadeOut; t = 0.f; }
		break;
	}

	case State::FadeOut: {
		float p = ImClamp(t / FADEOUT_DUR, 0.f, 1.f);
		alpha[0] = 1.f - p;
		d_shift = saved_d_shift;
		fade_mul = 1.f - p;
		if (t > FADEOUT_DUR + 0.1f) s = State::Done;
		break;
	}

	default: break;
	}

	fg->AddRectFilled({ 0.f, 0.f }, screen,
		IM_COL32(0, 0, 0, static_cast<int>(fade_mul * 180.f)));

	float x = base_x;
	for (int i = 0; i < N; i++) {
		if (alpha[i] > 0.005f) {
			char buf[2] = { TEXT[i], 0 };
			float rx = x + (i == 0 ? d_shift : 0.f);
			float ry = base_y + yoff[i];
			fg->AddText(font, FS, { rx, ry },
				IM_COL32(255, 255, 255, static_cast<int>(alpha[i] * 255.f)), buf);
		}
		x += cw[i];
	}

	return true;
}

void c_gui::render()
{
	static bool initialized_configs = false;

	if (render_intro())   
		return;

	intro_completed = true;

	if (!initialized_configs) {
		config::manager::init();
		scripts::manager::init();
		initialized_configs = true;
	}

	config::manager::tick_autosave(ImGui::GetIO().DeltaTime);

	if (GetAsyncKeyState(var->gui.menu_key) & 0x1)
		var->gui.menu_opened = !var->gui.menu_opened;

	ImGui::GetIO().MouseDrawCursor = false;

	var->gui.menu_alpha = ImClamp(var->gui.menu_alpha + (gui->fixed_speed(8.f) * (var->gui.menu_opened ? 1.f : -1.f)), 0.f, 1.f);

	if (var->gui.menu_alpha <= 0.01f)
		return;

	clr->apply();

	static const float s_dpi_values[] = { 0.75f, 1.0f, 1.25f, 1.5f, 1.75f, 2.0f };
	float dpi_scale = s_dpi_values[ImClamp(config::menu::dpi_scale_idx.value, 0, 5)];
	ImGui::GetIO().FontGlobalScale = dpi_scale;

	const ImGuiWindowFlags lock_flag = config::menu::lock_layout.value
		? ImGuiWindowFlags_NoResize
		: 0;

	gui->set_next_window_pos(ImVec2(GetIO().DisplaySize.x / 2 - (var->window.width) / 2, 20));
	gui->set_next_window_size(ImVec2(var->window.width, (elements->section.size.y + var->window.spacing.y * 2 - 1) * dpi_scale));
	gui->push_style_var(ImGuiStyleVar_Alpha, var->gui.menu_alpha);
	gui->begin("delusive.pro", nullptr, var->window.main_flags);
	{
		const ImVec2 pos = GetWindowPos();
		const ImVec2 size = GetWindowSize();
		ImDrawList* draw_list = GetWindowDrawList();
		ImGuiStyle* style = &GetStyle();

		{
			style->WindowPadding = var->window.padding * dpi_scale;
			style->PopupBorderSize = var->window.border_size * dpi_scale;
			style->WindowBorderSize = var->window.border_size * dpi_scale;
			style->ItemSpacing = var->window.spacing * dpi_scale;
			style->WindowShadowSize = var->window.shadow_enabled ? var->window.shadow_size * dpi_scale : 0.f;
			style->ScrollbarSize = var->window.scrollbar_size * dpi_scale;
			style->Colors[ImGuiCol_WindowShadow] = { clr->accent.Value.x, clr->accent.Value.y, clr->accent.Value.z, var->window.shadow_alpha };
		}

		{
			draw->rect(GetBackgroundDrawList(), pos - ImVec2(1, 1), pos + size + ImVec2(1, 1), draw->get_clr({0, 0, 0, 0.5f}));
			draw->rect_filled(draw_list, pos, pos + size, draw->get_clr(clr->window.background_one));
			draw->line(draw_list, pos + ImVec2(1, 1), pos + ImVec2(size.x - 1, 1), draw->get_clr(clr->accent), 1);
			draw->line(draw_list, pos + ImVec2(1, 2), pos + ImVec2(size.x - 1, 2), draw->get_clr(clr->accent, 0.4f), 1);
			draw->rect(draw_list, pos, pos + size, draw->get_clr(clr->window.stroke));
		}

		{
			gui->set_cursor_pos(style->ItemSpacing);
			gui->begin_group();
			{
				int section_count = IM_ARRAYSIZE(var->gui.current_section);
				int icons_count = IM_ARRAYSIZE(var->gui.section_icons); 

				for (int i = 0; i < section_count; i++)
				{
					const char* icon = "?";

					if (i < icons_count)
					{
						icon = var->gui.section_icons[i];
					}

					gui->section(icon, &var->gui.current_section[i]);
				}
			}
			gui->end_group();
		}

		{
			if (var->gui.current_section[0])
			{
				gui->set_next_window_size_constraints(ImVec2(400, 400), GetIO().DisplaySize);
				gui->begin("main", nullptr, var->window.flags | lock_flag);
				{
					draw->window_decorations();

					// ── Status + expires в titlebar ───────────────────────────────────
					{
						ImDrawList* draw_list = GetWindowDrawList();
						ImVec2 win_pos = GetWindowPos();
						float  win_width = GetWindowWidth();

						int         status_type = 1;
						std::string status_str;
						ImColor     status_color;

						if (status_type == 0) { status_str = "undetected";     status_color = ImColor(0.35f, 0.75f, 0.35f, 1.f); }
						else if (status_type == 1) { status_str = "use at own risk"; status_color = ImColor(0.85f, 0.65f, 0.25f, 1.f); }
						else { status_str = "detected";        status_color = ImColor(0.75f, 0.25f, 0.25f, 1.f); }

						const std::string sep_str = "  |  ";
						const std::string days_str = "expires: 999 days";

						const float status_w = var->font.tahoma->CalcTextSizeA(var->font.tahoma->FontSize, FLT_MAX, 0.f, status_str.c_str()).x;
						const float sep_w = var->font.tahoma->CalcTextSizeA(var->font.tahoma->FontSize, FLT_MAX, 0.f, sep_str.c_str()).x;
						const float days_w = var->font.tahoma->CalcTextSizeA(var->font.tahoma->FontSize, FLT_MAX, 0.f, days_str.c_str()).x;
						const float total_w = status_w + sep_w + days_w;

						const ImVec2 info_pos = win_pos + ImVec2(
							win_width - total_w - elements->content.window_padding.x,
							var->window.titlebar / 2.f - var->font.tahoma->FontSize / 2.f);

						draw->text_outline(draw_list, var->font.tahoma, var->font.tahoma->FontSize,
							info_pos,
							draw->get_clr(status_color), status_str.c_str());
						draw->text_outline(draw_list, var->font.tahoma, var->font.tahoma->FontSize,
							info_pos + ImVec2(status_w, 0),
							draw->get_clr(clr->widgets.text_inactive), sep_str.c_str());
						draw->text_outline(draw_list, var->font.tahoma, var->font.tahoma->FontSize,
							info_pos + ImVec2(status_w + sep_w, 0),
							draw->get_clr(clr->widgets.text_inactive), days_str.c_str());
					}

					// ── Сабтабы ───────────────────────────────────────────────────────
					{
						static int subtabs = 0;

						// Синхронизируем с Lua
						scripts::lua_active_subtab = subtabs;

						gui->set_cursor_pos(elements->content.window_padding + ImVec2(0, var->window.titlebar));
						gui->begin_group();
						{
							gui->sub_section("Aimbot", 0, subtabs, 5);
							gui->sub_section("Players", 1, subtabs, 5);
							gui->sub_section("Visuals", 2, subtabs, 5);
							gui->sub_section("Misc", 3, subtabs, 5);
							gui->sub_section("Player List", 4, subtabs, 5);
						}
						gui->end_group();

						gui->set_cursor_pos(elements->content.window_padding + ImVec2(0, var->window.titlebar + elements->section.height - 1));
						gui->begin_content();
						{
							const float col_width = (GetWindowWidth() - elements->content.window_padding.x * 2.f) / 2.f - 4.f;

							// ── Aimbot ────────────────────────────────────────────────
							if (subtabs == 0)
							{
								gui->begin_group();
								{
									gui->begin_child("Aim Assist", 2, 1);
									{
										static bool enabled = true; static int key = 0; static int mode = 0;
										gui->checkbox("Enabled", &enabled, &key, &mode);

										if (enabled) {
											static int fov = 90;
											gui->slider_int("Field Of View", &fov, 0, 100);

											static int fov_type = 0;
											const char* fov_type_items[2] = { "Static", "Dynamic" };
											gui->dropdown("FOV Type", &fov_type, fov_type_items, IM_ARRAYSIZE(fov_type_items), true);

											static float horizontal = 30.f;
											static float vertical = 12.f;
											gui->slider_float("Horizontal Smoothing", &horizontal, 0, 100);
											gui->slider_float("Vertical Smoothing", &vertical, 0, 100);

											static std::vector<int> checks = { 1, 1, 1 };
											const char* checks_items[3] = { "Team Check", "Alive Check", "Enemy Check" };
											gui->multi_dropdown("Checks", checks, checks_items, IM_ARRAYSIZE(checks_items));

											static std::vector<int> hitboxes = { 1, 0, 1, 0, 1, 0 };
											const char* hitboxes_items[6] = { "Head", "Neck", "Stomach", "Body", "Arms", "Legs" };
											gui->multi_dropdown("Hitboxes", hitboxes, hitboxes_items, IM_ARRAYSIZE(hitboxes_items));

											static bool randomize = false;
											gui->checkbox("Randomize Position", &randomize);

											static int hitscan_type = 0;
											gui->dropdown("Hitscan Type", &hitscan_type, fov_type_items, IM_ARRAYSIZE(fov_type_items));

											static bool readjustment = false; static int rkey = 0; static int rmode = 0;
											gui->checkbox("Readjustment", &readjustment, &rkey, &rmode);

											static bool deadzone = true;
											gui->checkbox("Dead Zone", &deadzone);
											if (deadzone) {
												static float dzone = 44.f;
												gui->slider_float("Dead Zone Size", &dzone, 0, 100, true);
											}

											static bool stutter = true;
											gui->checkbox("Stutter", &stutter);
											if (stutter) {
												static float stslider = 25.f;
												gui->slider_float("Stutter Amount", &stslider, 0, 100, true, "%.1ft");
											}
										}
									}
									gui->end_child();

									scripts::manager::draw_tab_column(0, 0, col_width);
								}
								gui->end_group();

								gui->sameline();

								gui->begin_group();
								{
									gui->begin_child("Trigger Assist", 2, 2);
									{
										static bool t_enabled = true; static int t_key = 0; static int t_mode = 0;
										gui->checkbox("Enabled", &t_enabled, &t_key, &t_mode);

										if (t_enabled) {
											static int delay = 15;
											static int interval = 75;
											gui->slider_int("Delay", &delay, 0, 500, false, "%dms");
											gui->slider_int("Interval", &interval, 0, 1000, false, "%dms");

											static std::vector<int> t_checks = { 1, 1, 1 };
											const char* t_checks_items[3] = { "Team Check", "Alive Check", "Enemy Check" };
											gui->multi_dropdown("Checks", t_checks, t_checks_items, IM_ARRAYSIZE(t_checks_items));

											static std::vector<int> t_hitboxes = { 1, 0, 1, 0, 1, 0 };
											const char* t_hitboxes_items[6] = { "Head", "Neck", "Stomach", "Body", "Arms", "Legs" };
											gui->multi_dropdown("Hitboxes", t_hitboxes, t_hitboxes_items, IM_ARRAYSIZE(t_hitboxes_items));

											static bool r_adj = false; static int r_key = 0; static int r_mode = 0;
											gui->checkbox("Readjustment", &r_adj, &r_key, &r_mode);
										}
									}
									gui->end_child();

									gui->begin_child("Misc", 2, 2);
									{
										static bool cursor = true;
										gui->checkbox("Cursor Offset", &cursor);
										if (cursor) {
											static int cx = 50; gui->slider_int("Offset X", &cx, 0, 100, true, "%dpx");
											static int cy = 50; gui->slider_int("Offset Y", &cy, 0, 100, true, "%dpx");
										}
									}
									gui->end_child();

									scripts::manager::draw_tab_column(0, 1, col_width);
								}
								gui->end_group();
							}

							// ── Players ───────────────────────────────────────────────
							if (subtabs == 1)
							{
								gui->begin_group();
								{
									// Ставим 2, чтобы блок занял половину экрана
									gui->begin_child("Player ESP", 2, 1);
									{
										// Боксы
										gui->checkbox("Bounding Box", &config::esp::players_box.value);
										if (config::esp::players_box.value) {
											gui->sameline();
											gui->label_color_edit("##box_clr", (float*)&config::esp::players_box_color.value.Value);

											const char* box_types[] = { "Normal", "Corner" };
											gui->dropdown("Box Type", &config::esp::players_box_type.value, box_types, 2);

											gui->checkbox("Box Fill", &config::esp::players_box_fill.value);
											gui->sameline();
											gui->label_color_edit("##box_fill_clr", (float*)&config::esp::players_box_fill_color.value.Value);
										}

										// Здоровье и скелет
										gui->checkbox("Health Bar", &config::esp::players_health.value);
										if (config::esp::players_health.value) {
											gui->sameline();
											gui->label_color_edit("##hp_clr", (float*)&config::esp::players_health_color.value.Value);
											gui->checkbox("Health Text", &config::esp::players_health_text.value);
										}

										gui->checkbox("Skeleton", &config::esp::players_skeleton.value);
										if (config::esp::players_skeleton.value) {
											gui->sameline();
											gui->label_color_edit("##skel_clr", (float*)&config::esp::players_skeleton_color.value.Value);
										}

										// Текстовая информация
										gui->checkbox("Name", &config::esp::players_name.value);
										if (config::esp::players_name.value) {
											gui->sameline();
											gui->label_color_edit("##name_clr", (float*)&config::esp::players_name_color.value.Value);

											const char* text_cases[] = { "Normal", "Upper", "Lower" };
											gui->dropdown("Name Case", &config::esp::players_name_case.value, text_cases, 3);
										}

										gui->checkbox("Distance", &config::esp::players_distance.value);
										if (config::esp::players_distance.value) {
											gui->sameline();
											gui->label_color_edit("##dist_clr", (float*)&config::esp::players_distance_color.value.Value);
											const char* text_cases[] = { "Normal", "Upper", "Lower" };
											gui->dropdown("Distance Case", &config::esp::players_distance_case.value, text_cases, 3);
										}

										// Оружие
										gui->checkbox("Weapon", &config::esp::players_weapon.value);
										if (config::esp::players_weapon.value) {
											gui->sameline();
											gui->label_color_edit("##weap_clr", (float*)&config::esp::players_weapon_color.value.Value);

											const char* weapon_types[] = { "Text", "Icons", "Flat Icons" };
											gui->dropdown("Weapon Type", &config::esp::players_weapon_type.value, weapon_types, 3);
											const char* text_cases[] = { "Normal", "Upper", "Lower" };
											gui->dropdown("Weapon Case", &config::esp::players_weapon_case.value, text_cases, 3);
										}
									}
									gui->end_child();
								}
								gui->end_group();

								gui->sameline();

								gui->begin_group();
								{
									// Ставим 2, чтобы блок встал во вторую половину экрана
									gui->begin_child("Filters & Settings", 2, 1);
									{
										// Основные фильтры
										gui->slider_float("Max Distance", &config::esp::players_max_render_distance.value, 0.f, 500.f, false, "%.0fm");

										gui->checkbox("Visible Only", &config::esp::players_only_visible.value);
										gui->checkbox("Show NPCs", &config::esp::players_show_npcs.value);
										gui->checkbox("Show Sleepers", &config::esp::players_show_sleepers.value);
										gui->checkbox("Show Wounded", &config::esp::players_show_wounded.value);
										gui->checkbox("Show Teammates", &config::esp::players_show_teammates.value);

										// Детализация скелета
										gui->slider_float("Skeleton Thickness", &config::esp::players_skeleton_thickness.value, 1.f, 5.f, false, "%.1f");
										if (config::esp::players_skeleton.value) {
											gui->checkbox("Skeleton Fingers", &config::esp::players_skeleton_fingers.value);
											gui->checkbox("Skeleton Joints", &config::esp::players_skeleton_joints.value);
										}

										// Флаги через мульти-дропдаун
										gui->checkbox("Status Flags", &config::esp::players_flags.value);
										if (config::esp::players_flags.value)
										{
											const char* flags_items[] = { "Visible", "Zooming", "Safezone", "Inside", "Mount", "Sleep", "Team" };
											gui->multi_dropdown("Select Flags", config::esp::players_flags_list.value, flags_items, 7);

											gui->label_color_edit("Visible Color", (float*)&config::esp::prev_flag_visible_color.value.Value);
											gui->label_color_edit("Zooming Color", (float*)&config::esp::prev_flag_zooming_color.value.Value);
											gui->label_color_edit("Safezone Color", (float*)&config::esp::prev_flag_safezone_color.value.Value);
											gui->label_color_edit("Inside Color", (float*)&config::esp::prev_flag_inside_color.value.Value);
											gui->label_color_edit("Mount Color", (float*)&config::esp::prev_flag_mount_color.value.Value);
											gui->label_color_edit("Sleep Color", (float*)&config::esp::prev_flag_sleep_color.value.Value);
											gui->label_color_edit("Team Color", (float*)&config::esp::prev_flag_team_color.value.Value);
										}
									}
									gui->end_child();
								}
								gui->end_group();
							}
							// ── Visuals ───────────────────────────────────────────────
							if (subtabs == 2)
							{
								const float sp = elements->content.spacing.y;

								const float h_env_child = ImGui::GetContentRegionAvail().y * 0.78f;
								const float h_weather_child = ImGui::GetContentRegionAvail().y * 0.35f;


								// ════════════════════════════════════════════
								//  LEFT COLUMN
								// ════════════════════════════════════════════
								gui->begin_group();
								{
									// ── World Environment ────────────────────
									gui->begin_child("World Environment", 2, 0, ImVec2(0, h_env_child));
									{
										// ── Time ────────────────────────────
										gui->checkbox("Time Changer", &config::visuals::worlds::visuals_time_change_time.value);
										if (config::visuals::worlds::visuals_time_change_time.value)
											gui->slider_float("Time Value", &config::visuals::worlds::visuals_time_change_time_value.value, 0.0f, 24.0f, false, "%.1f");

										ImGui::Separator();

										// ── Brightness ──────────────────────
										gui->checkbox("Bright Night", &config::visuals::worlds::visuals_sky_bright_night.value);
										if (config::visuals::worlds::visuals_sky_bright_night.value)
										{
											gui->slider_float("Ambient Mult", &config::visuals::worlds::visuals_sky_bright_night_ambient_multiplier.value, 0.f, 10.f, false, "%.1f");
											gui->slider_float("Reflect Mult", &config::visuals::worlds::visuals_sky_bright_night_reflection_multiplier.value, 0.f, 10.f, false, "%.1f");
										}
										gui->checkbox("Bright Cave", &config::visuals::worlds::visuals_sky_bright_cave.value);

										ImGui::Separator();

										// ── Atmosphere ───────────────────────
										gui->checkbox("Atmosphere Contrast", &config::visuals::worlds::atmosphere_contrast.value);
										if (config::visuals::worlds::atmosphere_contrast.value)
											gui->slider_float("Contrast", &config::visuals::worlds::atmosphere_contrast_value.value, 0.f, 5.f, false, "%.1f");

										gui->checkbox("Rayleigh Scattering", &config::visuals::worlds::rayleigh_multiplier.value);
										if (config::visuals::worlds::rayleigh_multiplier.value)
											gui->slider_float("Rayleigh", &config::visuals::worlds::rayleigh_multiplier_value.value, 0.0f, 10.0f, false, "%.1f");

										ImGui::Separator();

										// ── Orbit ────────────────────────────
										gui->checkbox("Latitude (Orbit)", &config::visuals::worlds::custom_latitude.value);
										if (config::visuals::worlds::custom_latitude.value)
											gui->slider_float("Latitude", &config::visuals::worlds::custom_latitude_value.value, -90.f, 90.f, false, "%.1f");

										gui->checkbox("Longitude (Orbit)", &config::visuals::worlds::custom_longitude.value);
										if (config::visuals::worlds::custom_longitude.value)
											gui->slider_float("Longitude", &config::visuals::worlds::custom_longitude_value.value, -180.f, 180.f, false, "%.1f");

										ImGui::Separator();

										// ── Sky Light & Shadow ───────────────
										gui->checkbox("Sky Light Intensity", &config::visuals::worlds::sky_light_intensity.value);
										if (config::visuals::worlds::sky_light_intensity.value)
										{
											gui->slider_float("Day Light", &config::visuals::worlds::sky_day_light_intensity_value.value, 0.0f, 10.0f, false, "%.1f");
											gui->slider_float("Night Light", &config::visuals::worlds::sky_night_light_intensity_value.value, 0.0f, 10.0f, false, "%.1f");
										}

										gui->checkbox("Sky Shadow Strength", &config::visuals::worlds::sky_shadow_strength.value);
										if (config::visuals::worlds::sky_shadow_strength.value)
										{
											gui->slider_float("Day Shadow", &config::visuals::worlds::sky_day_shadow_strength_value.value, 0.0f, 1.0f, false, "%.2f");
											gui->slider_float("Night Shadow", &config::visuals::worlds::sky_night_shadow_strength_value.value, 0.0f, 1.0f, false, "%.2f");
										}

										ImGui::Separator();

										gui->checkbox("Sky Tint", &config::visuals::worlds::sky_color_changer.value);
										if (config::visuals::worlds::sky_color_changer.value)
										{
											gui->sameline();
											float col[4] = { config::visuals::worlds::sky_color_changer_color.value.Value.x, config::visuals::worlds::sky_color_changer_color.value.Value.y, config::visuals::worlds::sky_color_changer_color.value.Value.z, config::visuals::worlds::sky_color_changer_color.value.Value.w };
											if (gui->color_edit(_("Sky Tint Color"), col, true))
												config::visuals::worlds::sky_color_changer_color.value = ImColor(col[0], col[1], col[2], col[3]);
										}

										// Cloud Tint
										gui->checkbox("Cloud Tint", &config::visuals::worlds::cloud_color_changer.value);
										if (config::visuals::worlds::cloud_color_changer.value)
										{
											gui->sameline();
											float col[4] = { config::visuals::worlds::cloud_color_changer_color.value.Value.x, config::visuals::worlds::cloud_color_changer_color.value.Value.y, config::visuals::worlds::cloud_color_changer_color.value.Value.z, config::visuals::worlds::cloud_color_changer_color.value.Value.w };
											if (gui->color_edit(_("Cloud Tint Color"), col, true))
												config::visuals::worlds::cloud_color_changer_color.value = ImColor(col[0], col[1], col[2], col[3]);
										}

										// Fog Tint
										gui->checkbox("Fog Tint", &config::visuals::worlds::fog_color_changer.value);
										if (config::visuals::worlds::fog_color_changer.value)
										{
											gui->sameline();
											float col[4] = { config::visuals::worlds::fog_color_changer_color.value.Value.x, config::visuals::worlds::fog_color_changer_color.value.Value.y, config::visuals::worlds::fog_color_changer_color.value.Value.z, config::visuals::worlds::fog_color_changer_color.value.Value.w };
											if (gui->color_edit(_("Fog Tint Color"), col, true))
												config::visuals::worlds::fog_color_changer_color.value = ImColor(col[0], col[1], col[2], col[3]);
										}

										// World Tint
										gui->checkbox("World Tint", &config::visuals::worlds::world_color_changer.value);
										if (config::visuals::worlds::world_color_changer.value)
										{
											gui->sameline();
											float col[4] = { config::visuals::worlds::world_color_changer_color.value.Value.x, config::visuals::worlds::world_color_changer_color.value.Value.y, config::visuals::worlds::world_color_changer_color.value.Value.z, config::visuals::worlds::world_color_changer_color.value.Value.w };
											if (gui->color_edit(_("World Tint Color"), col, true))
												config::visuals::worlds::world_color_changer_color.value = ImColor(col[0], col[1], col[2], col[3]);
										}

										ImGui::Separator();

										// ── Sun (hidden if No Sun is enabled) ─
										if (!config::visuals::weather::removals::visuals_weather_no_sun.value)
										{
											gui->checkbox("Override Sun", &config::visuals::worlds::sun_size_override.value);
											if (config::visuals::worlds::sun_size_override.value)
												gui->slider_float("Sun Size", &config::visuals::worlds::sun_size.value, 0.1f, 50.f, false, "%.1f");

											gui->checkbox("Sun Brightness", &config::visuals::worlds::sun_brightness_override.value);
											if (config::visuals::worlds::sun_brightness_override.value)
												gui->slider_float("Sun Bright", &config::visuals::worlds::sun_brightness.value, 0.1f, 50.f, false, "%.1f");

											ImGui::Separator();
										}

										// ── Moon ─────────────────────────────
										gui->checkbox("Override Moon", &config::visuals::worlds::moon_size_override.value);
										if (config::visuals::worlds::moon_size_override.value)
											gui->slider_float("Moon Size", &config::visuals::worlds::moon_size.value, 0.1f, 50.f, false, "%.1f");

										gui->checkbox("Moon Brightness", &config::visuals::worlds::moon_brightness_override.value);
										if (config::visuals::worlds::moon_brightness_override.value)
											gui->slider_float("Moon Bright", &config::visuals::worlds::moon_brightness.value, 0.1f, 50.f, false, "%.1f");

										gui->checkbox("Moon Phase", &config::visuals::worlds::moon_phase_override.value);
										if (config::visuals::worlds::moon_phase_override.value)
											gui->slider_float("Phase", &config::visuals::worlds::moon_phase.value, 0.f, 1.f, false, "%.2f");

										gui->checkbox("Moon Halo", &config::visuals::worlds::moon_halo.value);
										if (config::visuals::worlds::moon_halo.value)
										{
											gui->slider_float("Halo Size", &config::visuals::worlds::moon_halo_size.value, 0.1f, 15.f, false, "%.1f");
											gui->slider_float("Halo Bright", &config::visuals::worlds::moon_halo_brightness.value, 0.1f, 15.f, false, "%.1f");
										}

										const char* moon_positions[] = { "Opposite To Sun", "Realistic" };
										gui->dropdown("Moon Position", &config::visuals::worlds::moon_position.value, moon_positions, 2);

										ImGui::Separator();

										// ── Stars ────────────────────────────
										gui->checkbox("Night Stars", &config::visuals::worlds::night_stars.value);
										if (config::visuals::worlds::night_stars.value)
										{
											gui->slider_float("Stars Size", &config::visuals::worlds::night_stars_size.value, 0.1f, 15.f, false, "%.1f");
											gui->slider_float("Stars Bright", &config::visuals::worlds::night_stars_brightness.value, 0.1f, 15.f, false, "%.1f");
										}

										const char* star_positions[] = { "Static", "Rotating" };
										gui->dropdown("Stars Position", &config::visuals::worlds::night_stars_position.value, star_positions, 2);
									}
									gui->end_child();

									// ── Physical Layers ──────────────────────
									gui->begin_child("Physical Layers", 2, 0, ImVec2(0, ImGui::GetContentRegionAvail().y - elements->content.padding.y));
									{
										gui->checkbox("Enable Culling", &config::visuals::layers::layers_enable.value);
										ImGui::SameLine(180.f * dpi_scale);
										gui->keybind("Layers Bind", &config::visuals::layers::layers_bind.value, &config::visuals::layers::layers_mode.value);

										if (config::visuals::layers::layers_enable.value)
										{
											std::vector<int> layer_removals = {
												config::visuals::layers::layers_remove_water.value ? 1 : 0,
												config::visuals::layers::layers_remove_ragdolls.value ? 1 : 0,
												config::visuals::layers::layers_remove_buildings.value ? 1 : 0,
												config::visuals::layers::layers_remove_terrain.value ? 1 : 0,
												config::visuals::layers::layers_remove_cargoship.value ? 1 : 0,
												config::visuals::layers::layers_remove_trees.value ? 1 : 0
											};
											const char* layer_labels[] = { "Water", "Ragdolls", "Buildings", "Terrain", "Cargo Ship", "Trees" };
											gui->multi_dropdown("Remove Layers", layer_removals, layer_labels, 6);

											config::visuals::layers::layers_remove_water.value = layer_removals[0] != 0;
											config::visuals::layers::layers_remove_ragdolls.value = layer_removals[1] != 0;
											config::visuals::layers::layers_remove_buildings.value = layer_removals[2] != 0;
											config::visuals::layers::layers_remove_terrain.value = layer_removals[3] != 0;
											config::visuals::layers::layers_remove_cargoship.value = layer_removals[4] != 0;
											config::visuals::layers::layers_remove_trees.value = layer_removals[5] != 0;
										}
									}
									gui->end_child();

									scripts::manager::draw_tab_column(2, 0, col_width);
								}
								gui->end_group();

								gui->sameline();

								// ════════════════════════════════════════════
								//  RIGHT COLUMN
								// ════════════════════════════════════════════
								gui->begin_group();
								{
									// ── Atmosphere & Weather ─────────────────
									gui->begin_child("Atmosphere & Weather", 2, 1, ImVec2(0, h_weather_child));
									{
										std::vector<int> weather_removals = {
											config::visuals::weather::removals::visuals_weather_no_clouds.value ? 1 : 0,
											config::visuals::weather::removals::visuals_weather_no_thunder.value ? 1 : 0,
											config::visuals::weather::removals::visuals_weather_no_wind.value ? 1 : 0,
											config::visuals::weather::removals::visuals_weather_rainbows.value ? 1 : 0,
											config::visuals::weather::removals::visuals_weather_no_fog.value ? 1 : 0,
											config::visuals::weather::removals::visuals_weather_no_rain.value ? 1 : 0,
											config::visuals::weather::removals::visuals_weather_no_sun.value ? 1 : 0
										};
										const char* weather_labels[] = { "No Clouds", "No Thunder", "No Wind", "Rainbows", "No Fog", "No Rain", "No Sun" };
										gui->multi_dropdown("Weather Removals", weather_removals, weather_labels, 7);

										config::visuals::weather::removals::visuals_weather_no_clouds.value = weather_removals[0] != 0;
										config::visuals::weather::removals::visuals_weather_no_thunder.value = weather_removals[1] != 0;
										config::visuals::weather::removals::visuals_weather_no_wind.value = weather_removals[2] != 0;
										config::visuals::weather::removals::visuals_weather_rainbows.value = weather_removals[3] != 0;
										config::visuals::weather::removals::visuals_weather_no_fog.value = weather_removals[4] != 0;
										config::visuals::weather::removals::visuals_weather_no_rain.value = weather_removals[5] != 0;
										config::visuals::weather::removals::visuals_weather_no_sun.value = weather_removals[6] != 0;

										ImGui::Separator();

										// Fog (hidden if No Fog is active)
										if (!config::visuals::weather::removals::visuals_weather_no_fog.value)
										{
											gui->checkbox("Modify Fog", &config::visuals::weather::modify_fog.value);
											if (config::visuals::weather::modify_fog.value)
												gui->slider_float("Fog Level", &config::visuals::weather::modify_fog_value.value, 0.f, 1.f, false, "%.2f");
										}

										// Snow & Rain (hidden if No Rain is active)
										if (!config::visuals::weather::removals::visuals_weather_no_rain.value)
										{
											gui->checkbox("Modify Snow", &config::visuals::weather::modify_snow.value);
											if (config::visuals::weather::modify_snow.value)
												gui->slider_float("Snow Level", &config::visuals::weather::modify_snow_value.value, 0.f, 1.f, false, "%.2f");

											gui->checkbox("Modify Rain", &config::visuals::weather::modify_rain.value);
											if (config::visuals::weather::modify_rain.value)
												gui->slider_float("Rain Level", &config::visuals::weather::modify_rain_value.value, 0.f, 1.f, false, "%.2f");
										}
									}
									gui->end_child();

									// ── Custom Clouds ────────────────────────
									gui->begin_child("Clouds", 2, 1, ImVec2(0, ImGui::GetContentRegionAvail().y - elements->content.padding.y));
									{
										if (!config::visuals::weather::removals::visuals_weather_no_clouds.value)
										{
											gui->checkbox("Custom Clouds", &config::visuals::worlds::custom_clouds.value);
											if (config::visuals::worlds::custom_clouds.value)
											{
												ImGui::Separator();
												gui->slider_float("Cloud Size", &config::visuals::worlds::cloud_size.value, 0.1f, 50.0f, false, "%.1f");
												gui->slider_float("Cloud Opacity", &config::visuals::worlds::cloud_opacity.value, 0.0f, 1.0f, false, "%.2f");
												gui->slider_float("Cloud Coverage", &config::visuals::worlds::cloud_coverage.value, 0.0f, 1.0f, false, "%.2f");
												gui->slider_float("Cloud Sharpness", &config::visuals::worlds::cloud_sharpness.value, 0.0f, 1.0f, false, "%.2f");
												gui->slider_float("Cloud Coloring", &config::visuals::worlds::cloud_coloring.value, 0.0f, 1.0f, false, "%.2f");
												gui->slider_float("Cloud Attenuation", &config::visuals::worlds::cloud_attenuation.value, 0.0f, 1.0f, false, "%.2f");
												gui->slider_float("Cloud Saturation", &config::visuals::worlds::cloud_saturation.value, 0.0f, 1.0f, false, "%.2f");
												gui->slider_float("Cloud Scattering", &config::visuals::worlds::cloud_scattering.value, 0.1f, 10.0f, false, "%.1f");
												gui->slider_float("Cloud Brightness", &config::visuals::worlds::cloud_brightness.value, 0.1f, 10.0f, false, "%.1f");
											}
										}
										else
										{
											ImGui::TextDisabled("Clouds are disabled in Weather Removals.");
										}
									}
									gui->end_child();

									scripts::manager::draw_tab_column(2, 1, col_width);
								}
								gui->end_group();
							}
							// ── Misc ──────────────────────────────────────────────────
							if (subtabs == 3)
							{
								const float h_env_child_left = ImGui::GetContentRegionAvail().y * 0.35f;
								const float h_env_child_right = ImGui::GetContentRegionAvail().y * 0.45f;

								// ── LEFT COLUMN ──────────────────────────────
								gui->begin_group();
								{
									// ── Exploits ─────────────────────────────
									gui->begin_child("Exploits", 2, 0, ImVec2(0, h_env_child_left));
									{
										gui->checkbox("Fast Loot", &config::exploits_fast_loot.value);

										gui->checkbox("Auto Reload", &config::misc::automatic::misc_automatic_auto_reload.value);

										gui->checkbox("Auto Heal", &config::misc::automatic::misc_automatic_auto_heal.value);
										gui->checkbox("Ore Farm Assist", &config::misc::automatic::ore_assist_farm.value);
										gui->checkbox("Tree Farm Assist", &config::misc::automatic::tree_assist_farm.value);

										gui->checkbox("Fake Admin", &config::misc::exploits::exploits_admin_flag.value);
										if (ImGui::IsItemHovered())
										{
											ImGui::BeginTooltip();
											ImGui::Text("Adds an admin icon next to your name.\nDoes not grant real admin privileges.");
											ImGui::EndTooltip();
										}

										gui->checkbox("Anti-Fly Kick", &config::misc::exploits::exploits_anti_fly_hack_kick.value);
										if (ImGui::IsItemHovered())
										{
											ImGui::BeginTooltip();
											ImGui::Text("Prevents the server from kicking\nyou for fly-hack detection.");
											ImGui::EndTooltip();
										}

										gui->checkbox("Extended Melee", &config::misc::exploits::exploits_extended_melee.value);
										if (ImGui::IsItemHovered())
										{
											ImGui::BeginTooltip();
											ImGui::Text("Extends melee weapon reach.");
											ImGui::EndTooltip();
										}

										gui->checkbox("Name Spoofer", &config::misc::exploits::exploits_name_spoofer.value);
										if (ImGui::IsItemHovered())
										{
											ImGui::BeginTooltip();
											ImGui::Text("Spoofs your username to a random one.\nTo apply, stay in main menu or rejoin the server.");
											ImGui::EndTooltip();
										}

										gui->checkbox("Attack In Air", &config::misc::exploits::misc_modify_can_attack.value);
										gui->checkbox("Aim With Heavy Armor", &config::misc::exploits::exploits_unlock_aim_on_jugger_set.value);
										gui->checkbox("Attack In Vehicles", &config::misc::exploits::exploits_can_attack_in_vehicles.value);

										gui->checkbox("Traps Exploit", &config::misc::exploits::exploits_traps_exploit.value);
										if (config::misc::exploits::exploits_traps_exploit.value) {
											gui->sameline();
											gui->keybind("Traps Key", &config::misc::exploits::exploits_traps_exploit_key.value, &config::misc::exploits::exploits_traps_exploit_keymode.value);

											gui->slider_float("Traps Amount", &config::misc::exploits::exploits_traps_exploit_amount.value, 0.05f, 2.0f, false, "%.2f");
										}

										gui->checkbox("Fake Lag", &config::misc::exploits::exploits_fake_lag.value);
										if (config::misc::exploits::exploits_fake_lag.value) {
											gui->slider_float("FL Cooldown", &config::misc::exploits::exploits_fake_lag_cooldown.value, 0.1f, 5.0f, false, "%.2f");
											gui->slider_float("FL Duration", &config::misc::exploits::exploits_fake_lag_duration.value, 0.1f, 1.0f, false, "%.2f");
											gui->slider_float("FL Min", &config::misc::exploits::exploits_fake_lag_min.value, 0.0f, 0.5f, false, "%.2f");
											gui->slider_float("FL Max", &config::misc::exploits::exploits_fake_lag_max.value, 0.0f, 1.0f, false, "%.2f");
										}
									}
									gui->end_child();

									// ── View ─────────────────────────────────
									gui->begin_child("View", 2, 0, ImVec2(0, ImGui::GetContentRegionAvail().y - elements->content.padding.y));
									{
										// FOV
										gui->checkbox("FOV Changer", &config::misc::view::misc_fov_changer.value);
										if (config::misc::view::misc_fov_changer.value)
											gui->slider_float("FOV Amount", &config::misc::view::misc_fov_changer_amount.value, 60.f, 150.f, false, "%.0f");

										ImGui::Separator();

										// Zoom
										gui->checkbox("Zoom Changer", &config::misc::view::misc_zoom_changer.value, &config::misc::view::misc_zoom_key.value, &config::misc::view::misc_zoom_key_mode.value);
										if (config::misc::view::misc_zoom_changer.value)
											gui->slider_float("Zoom FOV", &config::misc::view::misc_zoom_value.value, 5.f, 89.f, false, "%.0f");

										ImGui::Separator();

										// Aspect Ratio
										gui->checkbox("Aspect Ratio", &config::misc::view::misc_aspect_ratio_changer.value);
										if (config::misc::view::misc_aspect_ratio_changer.value)
											gui->slider_float("Aspect Value", &config::misc::view::misc_aspect_ratio_value.value, 0.5f, 3.0f, false, "%.3f");

										ImGui::Separator();

										// Camera
										gui->checkbox("Camera Mode", &config::misc::view::misc_camera_mode.value, &config::misc::view::misc_camera_mode_key.value, &config::misc::view::misc_camera_mode_key_mode.value);
										if (config::misc::view::misc_camera_mode.value)
										{
											const char* cam_modes[] = { "First Person", "Third Person", "Eyes", "First Person (Arms)" };
											gui->dropdown("Camera Mode Type", &config::misc::view::misc_camera_mode_value.value, cam_modes, 4);
											gui->slider_float("Camera Distance", &config::misc::view::misc_camera_mode_dist.value, 0.f, 10.f, false, "%.1f");
										}

										gui->checkbox("Camera to Bullet", &config::misc::view::misc_camera_to_bullet.value, &config::misc::view::misc_camera_to_bullet_key.value, &config::misc::view::misc_camera_to_bullet_key_mode.value);
										if (ImGui::IsItemHovered())
										{
											ImGui::BeginTooltip();
											ImGui::Text("Moves the camera to follow\nthe bullet trajectory on shot.");
											ImGui::EndTooltip();
										}
									}
									gui->end_child();

									scripts::manager::draw_tab_column(3, 0, col_width);
								}
								gui->end_group();

								gui->sameline();

								// ── RIGHT COLUMN ─────────────────────────────
								gui->begin_group();
								{
									// ── Viewmodel ────────────────────────────
									gui->begin_child("Viewmodel", 2, 1, ImVec2(0, h_env_child_right));
									{
										gui->checkbox("Enable Features", &config::misc::viewmodel::misc_viewmodel.value);
										if (ImGui::IsItemHovered())
										{
											ImGui::BeginTooltip();
											ImGui::Text("Master toggle for all\nviewmodel modifications.");
											ImGui::EndTooltip();
										}

										if (config::misc::viewmodel::misc_viewmodel.value)
										{
											ImGui::Separator();

											gui->checkbox("Viewmodel Changer", &config::misc::viewmodel::misc_viewmodel_changer.value);
											if (config::misc::viewmodel::misc_viewmodel_changer.value)
											{
												gui->slider_float("Offset X", &config::misc::viewmodel::misc_viewmodel_X.value, -5.f, 5.f, false, "%.2f");
												gui->slider_float("Offset Y", &config::misc::viewmodel::misc_viewmodel_Y.value, -5.f, 5.f, false, "%.2f");
												gui->slider_float("Offset Z", &config::misc::viewmodel::misc_viewmodel_Z.value, -5.f, 5.f, false, "%.2f");
											}

											ImGui::Separator();

											gui->checkbox("No Bob", &config::misc::viewmodel::misc_no_viewmodel_bob.value);
											gui->checkbox("No Sway", &config::misc::viewmodel::misc_no_viewmodel_sway.value);
											gui->checkbox("No Lower", &config::misc::viewmodel::misc_no_viewmodel_lower.value);
											gui->checkbox("No Attack Animation", &config::misc::viewmodel::misc_no_attack_animation.value);
										}
									}
									gui->end_child();

									// ── Movement ─────────────────────────────
									gui->begin_child("Movement", 2, 1, ImVec2(0, ImGui::GetContentRegionAvail().y - elements->content.padding.y));
									{
										gui->checkbox("No Melee Slowdown", &config::misc::movement::disable_melee_slow_down.value);

										ImGui::Separator();

										// Walk exploits
										gui->checkbox("Walk Through Trees", &config::misc::movement::exploits_walk_through_trees.value);
										gui->checkbox("Walk Through Players", &config::misc::movement::exploits_walk_through_players.value);
										gui->checkbox("Walk On Water", &config::misc::movement::exploits_walk_on_water.value);

										ImGui::Separator();

										// Movement extras
										gui->checkbox("Omni Sprint", &config::misc::movement::exploits_omni_sprint.value);
										if (ImGui::IsItemHovered())
										{
											ImGui::BeginTooltip();
											ImGui::Text("Allows sprinting in any direction.");
											ImGui::EndTooltip();
										}

										gui->checkbox("Infinite Jump", &config::misc::movement::misc_infinite_jump.value);

										gui->checkbox("Climb Assist", &config::misc::movement::misc_climb_assist.value);
										if (config::misc::movement::misc_climb_assist.value)
											gui->checkbox("Bypass High Walls", &config::misc::movement::misc_climb_assist_bypass.value);

										gui->checkbox("Always Sprint", &config::misc::movement::misc_always_sprint.value);

										ImGui::Separator();

										gui->checkbox("Suicide", &config::misc::movement::misc_suicide.value, &config::misc::movement::misc_suicide_key.value, nullptr);
										if (ImGui::IsItemHovered())
										{
											ImGui::BeginTooltip();
											ImGui::Text("Instantly kills your character\nwhen the keybind is pressed.");
											ImGui::EndTooltip();
										}
									}
									gui->end_child();

									scripts::manager::draw_tab_column(3, 1, col_width);
								}
								gui->end_group();
							}
							// ── Player List ───────────────────────────────────────────
							if (subtabs == 4)
							{
								gui->push_style_var(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
								gui->push_style_var(ImGuiStyleVar_ItemSpacing, elements->content.spacing);

								gui->begin_def_child("player list",
									ImVec2(GetWindowWidth() - elements->content.window_padding.x * 2,
										GetContentRegionAvail().y - elements->content.window_padding.y * 2),
									0, ImGuiWindowFlags_NoMove);
								{
									ImGui::PushFont(var->font.tahoma);

									static int selected_row = -1;

									gui->push_style_color(ImGuiCol_TableBorderLight, draw->get_clr(clr->window.stroke));
									gui->push_style_color(ImGuiCol_TableBorderStrong, draw->get_clr(clr->window.stroke));
									gui->push_style_color(ImGuiCol_TableRowBg, draw->get_clr(clr->window.background_one));
									gui->push_style_color(ImGuiCol_TableRowBgAlt, draw->get_clr(clr->window.background_one));
									gui->push_style_color(ImGuiCol_Text, draw->get_clr(clr->widgets.text_inactive));

									if (gui->begin_table("PlayerTable", 3,
										ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg,
										ImVec2(GetContentRegionAvail().x - 1, 0)))
									{
										// Заголовки
										gui->table_next_row();
										gui->table_set_column_index(0); ImGui::Text("Name");
										gui->table_set_column_index(1); ImGui::Text("Position");
										gui->table_set_column_index(2); ImGui::Text("Info");

										for (int row = 0; row < 10; row++)
										{
											gui->table_next_row();

											gui->table_set_column_index(0);
											{
												if (selected_row == row)
													gui->push_style_color(ImGuiCol_Text, draw->get_clr(clr->accent));

												ImGui::Text("Player %d", row + 1);

												if (selected_row == row)
													gui->pop_style_color();

												if (ImGui::IsItemClicked())
													selected_row = row;
											}

											gui->table_set_column_index(1);
											ImGui::Text("%.0f, %.0f, %.0f",
												100.f * (row + 1), 50.f * row, 200.f - row * 10.f);

											gui->table_set_column_index(2);
											ImGui::Text("HP: %d%%", 100 - row * 8);
										}

										gui->end_table();

										if (selected_row >= 0)
											ImGui::Text("Selected: Player %d", selected_row + 1);
									}

									gui->pop_style_color(5);
									ImGui::PopFont();
								}
								gui->end_def_child();
								gui->pop_style_var(2);
							}
						}
						gui->end_content();
					}
				}
				gui->end();
			}

			if (var->gui.current_section[1])
			{
				gui->set_next_window_size(ImVec2(450, 500), ImGuiCond_Always); 
				gui->begin("esp preview", nullptr, var->window.flags | lock_flag | ImGuiWindowFlags_NoResize);
				{
					draw->window_decorations();

					gui->set_cursor_pos(elements->content.window_padding + ImVec2(0, var->window.titlebar + 1));
					gui->begin_content();
					{
						ImVec2 canvas_min = ImGui::GetCursorScreenPos();
						ImVec2 canvas_max = canvas_min + ImGui::GetContentRegionAvail();

						canvas_max.y -= elements->content.window_padding.y;

						esp_preview::draw(canvas_min, canvas_max);

						ImGui::Dummy(canvas_max - canvas_min);
					}
					gui->end_content();
				}
				gui->end();
			}

			if (var->gui.current_section[2])
			{
				gui->set_next_window_size_constraints(ImVec2(500, 450), GetIO().DisplaySize);
				gui->begin("scripts", nullptr, var->window.flags | lock_flag);
				{
					draw->window_decorations();

					gui->set_cursor_pos(elements->content.window_padding + ImVec2(0, var->window.titlebar + 1));
					gui->begin_content();
					{
						ImGui::PushFont(var->font.tahoma);

						static int selected_script_idx = -1;
						static char edit_buffer[16 * 1024] = ""; 
						static char new_script_name[32] = "";
						static bool refresh_code_view = false;

						gui->begin_group();
						{
							gui->begin_child("Loaded Scripts", 2, 1);
							{
								if (gui->button("Refresh Scripts List")) {
									scripts::manager::refresh_list();
									selected_script_idx = -1;
									memset(edit_buffer, 0, sizeof(edit_buffer));
								}

								static bool  show_security_confirm = false;
								static float confirm_timer = 0.f;
								static bool  confirm_ready = false;

								bool& sec = scripts::lua_security_enabled;

								bool checkbox_clicked = gui->checkbox("Lua Security", &sec);
								if (checkbox_clicked) {
									if (!sec) {
										sec = true;
										show_security_confirm = true;
										confirm_timer = 10.f;
										confirm_ready = false;
									}
									else {
										for (auto& sc : scripts::manager::script_list)
											if (sc.loaded) scripts::manager::load(sc);
									}
								}

								if (ImGui::IsItemHovered()) {
									ImGui::BeginTooltip();
									ImGui::PushStyleColor(ImGuiCol_Text,
										sec ? ImVec4(0.35f, 0.75f, 0.35f, 1.f)
										: ImVec4(0.85f, 0.35f, 0.35f, 1.f));
									ImGui::Text(sec ? "Security ON  -  io / os / ffi / debug are blocked"
										: "Security OFF -  scripts have full system access");
									ImGui::PopStyleColor();
									ImGui::EndTooltip();
								}

								if (!sec) {
									ImGui::SameLine();
									ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.6f, 0.1f, 1.f));
									ImGui::Text("  [!] Untrusted scripts can access system APIs");
									ImGui::PopStyleColor();
								}

								if (show_security_confirm)
								{
									ImDrawList* fg = ImGui::GetForegroundDrawList();
									ImGuiIO& io = ImGui::GetIO();

									const float fsz = var->font.tahoma->FontSize;
									const float pad = 10.f;

									const float w = 280.f;
									const float bar_h = fsz + pad * 2.f;   
									const float line_h = fsz + 4.f;
									const float prog_h = 4.f;
									const float btn_h = fsz + 10.f;
									const float body_h = pad + line_h * 3.f + pad * 2.f + prog_h + pad * 2.f + btn_h + pad;
									const float total_h = bar_h + body_h;

									auto gc = [&](ImColor color, float alpha = -1.f) -> ImU32 {
										const float a = (alpha < 0.f ? color.Value.w : alpha);
										return draw->get_clr(color, a);
										};

									const ImVec2 box_min = ImVec2(
										floorf((io.DisplaySize.x - w) * 0.5f),
										floorf((io.DisplaySize.y - total_h) * 0.5f));
									const ImVec2 box_max = box_min + ImVec2(w, total_h);

									const bool hovered = io.MousePos.x >= box_min.x && io.MousePos.x <= box_max.x
										&& io.MousePos.y >= box_min.y && io.MousePos.y <= box_max.y;

									fg->AddRectFilled(
										ImVec2(0.f, 0.f),
										io.DisplaySize,
										IM_COL32(0, 0, 0, 140));

									draw->rect_filled(fg, box_min, box_max, gc(clr->window.background_one));

									fg->AddLine(
										box_min + ImVec2(1.f, 0.f),
										ImVec2(box_max.x - 1.f, box_min.y),
										gc(clr->accent), 1.f);
									fg->AddLine(
										box_min + ImVec2(1.f, 1.f),
										ImVec2(box_max.x - 1.f, box_min.y + 1.f),
										gc(clr->accent, 0.35f), 1.f);

									draw->rect(fg, box_min, box_max,
										gc(hovered ? clr->accent : clr->window.stroke));

									const char* title = "Disable Lua Security?";
									const float title_w = var->font.tahoma->CalcTextSizeA(fsz, FLT_MAX, 0.f, title).x;
									draw->text_outline(fg, var->font.tahoma, fsz,
										ImVec2(floorf(box_min.x + (w - title_w) * 0.5f),
											floorf(box_min.y + pad)),
										gc(clr->accent), title);

									fg->AddLine(
										ImVec2(box_min.x + 1.f, box_min.y + bar_h),
										ImVec2(box_max.x - 1.f, box_min.y + bar_h),
										gc(clr->window.stroke), 1.f);

									float cy = box_min.y + bar_h + pad;
									const float cx = box_min.x + pad;
									const float content_w = w - pad * 2.f;

									const char* lines[] = {
										"Scripts will gain access to:",
										"io  |  os  |  ffi  |  debug",
										"Only load scripts you trust."
									};
									const ImU32 line_colors[] = {
										gc(clr->widgets.text),
										gc(clr->accent),
										gc(clr->widgets.text_inactive)
									};

									for (int i = 0; i < 3; i++) {
										const float lw = var->font.tahoma->CalcTextSizeA(fsz, FLT_MAX, 0.f, lines[i]).x;
										draw->text_outline(fg, var->font.tahoma, fsz,
											ImVec2(floorf(box_min.x + (w - lw) * 0.5f), floorf(cy)),
											line_colors[i], lines[i]);
										cy += line_h;
									}
									cy += pad;

									if (confirm_timer > 0.f) {
										confirm_timer -= io.DeltaTime;
										if (confirm_timer <= 0.f) { confirm_timer = 0.f; confirm_ready = true; }
									}

									const float fraction = confirm_ready ? 1.f : (1.f - confirm_timer / 10.f);

									fg->AddRectFilled(
										ImVec2(cx, cy),
										ImVec2(cx + content_w, cy + prog_h),
										gc(clr->window.background_two), 2.f);
									fg->AddRectFilled(
										ImVec2(cx, cy),
										ImVec2(floorf(cx + content_w * fraction), cy + prog_h),
										gc(confirm_ready ? clr->accent : clr->widgets.text_inactive, 0.8f), 2.f);

									cy += prog_h + 4.f;

									char timer_str[32];
									if (!confirm_ready) ImFormatString(timer_str, sizeof(timer_str), "%.0f sec", ceilf(confirm_timer));
									else                ImFormatString(timer_str, sizeof(timer_str), "Ready");
									const float tw = var->font.tahoma->CalcTextSizeA(fsz, FLT_MAX, 0.f, timer_str).x;
									draw->text_outline(fg, var->font.tahoma, fsz,
										ImVec2(floorf(box_min.x + (w - tw) * 0.5f), floorf(cy)),
										gc(confirm_ready ? clr->accent : clr->widgets.text_inactive), timer_str);

									cy += line_h + pad;

									const float gap = 6.f;
									const float btn_w = (content_w - gap) * 0.5f;

									const ImVec2 db_min = ImVec2(floorf(cx), floorf(cy));
									const ImVec2 db_max = ImVec2(floorf(cx + btn_w), floorf(cy + btn_h));

									const bool db_hov = confirm_ready
										&& io.MousePos.x >= db_min.x && io.MousePos.x <= db_max.x
										&& io.MousePos.y >= db_min.y && io.MousePos.y <= db_max.y;
									const bool db_clk = db_hov && io.MouseClicked[0];

									const float db_a = confirm_ready ? 1.f : 0.3f;
									fg->AddRectFilled(db_min, db_max,
										gc(db_hov ? clr->accent : clr->window.background_two, db_a), 3.f);
									draw->rect(fg, db_min, db_max,
										gc(confirm_ready ? clr->accent : clr->window.stroke, db_a));

									const char* db_lbl = "Disable Security";
									const float db_lw = var->font.tahoma->CalcTextSizeA(fsz, FLT_MAX, 0.f, db_lbl).x;
									draw->text_outline(fg, var->font.tahoma, fsz,
										ImVec2(floorf(db_min.x + (btn_w - db_lw) * 0.5f),
											floorf(db_min.y + (btn_h - fsz) * 0.5f)),
										gc(clr->widgets.text, db_a), db_lbl);

									if (db_clk) {
										sec = false;
										show_security_confirm = false;
										for (auto& sc : scripts::manager::script_list)
											if (sc.loaded) scripts::manager::load(sc);
									}

									const ImVec2 cb_min = ImVec2(floorf(cx + btn_w + gap), floorf(cy));
									const ImVec2 cb_max = ImVec2(floorf(cx + btn_w + gap + btn_w), floorf(cy + btn_h));

									const bool cb_hov = io.MousePos.x >= cb_min.x && io.MousePos.x <= cb_max.x
										&& io.MousePos.y >= cb_min.y && io.MousePos.y <= cb_max.y;
									const bool cb_clk = cb_hov && io.MouseClicked[0];

									fg->AddRectFilled(cb_min, cb_max,
										gc(cb_hov ? clr->widgets.stroke_two : clr->window.background_two), 3.f);
									draw->rect(fg, cb_min, cb_max,
										gc(cb_hov ? clr->accent : clr->window.stroke));

									const char* cb_lbl = "Cancel";
									const float cb_lw = var->font.tahoma->CalcTextSizeA(fsz, FLT_MAX, 0.f, cb_lbl).x;
									draw->text_outline(fg, var->font.tahoma, fsz,
										ImVec2(floorf(cb_min.x + (btn_w - cb_lw) * 0.5f),
											floorf(cb_min.y + (btn_h - fsz) * 0.5f)),
										gc(clr->widgets.text), cb_lbl);

									if (cb_clk) {
										show_security_confirm = false;
										sec = true;
									}
								}

								ImGui::Dummy(ImVec2(0, 4.f));
								ImGui::Separator();
								ImGui::Dummy(ImVec2(0, 4.f));

								gui->push_style_color(ImGuiCol_TableBorderLight, draw->get_clr(clr->window.stroke));
								gui->push_style_color(ImGuiCol_TableBorderStrong, draw->get_clr(clr->window.stroke));
								gui->push_style_color(ImGuiCol_TableRowBg, draw->get_clr(clr->window.background_one));

								if (gui->begin_table("ScriptsTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg, ImVec2(GetContentRegionAvail().x - 1, 0)))
								{
									gui->table_next_row();
									gui->table_set_column_index(0); ImGui::Text("Script Name");
									gui->table_set_column_index(1); ImGui::Text("Size");
									gui->table_set_column_index(2); ImGui::Text("Status");

									for (size_t i = 0; i < scripts::manager::script_list.size(); i++)
									{
										auto& sc = scripts::manager::script_list[i];
										gui->table_next_row();

										gui->table_set_column_index(0);
										{
											if (selected_script_idx == (int)i)
												gui->push_style_color(ImGuiCol_Text, draw->get_clr(clr->accent));

											ImGui::Text("%s", sc.name.c_str());

											if (selected_script_idx == (int)i)
												gui->pop_style_color();

											if (IsItemClicked()) {
												selected_script_idx = (int)i;
												strcpy_s(edit_buffer, sc.content.c_str());
											}
										}

										gui->table_set_column_index(1);
										ImGui::Text("%s", sc.size_str.c_str());

										gui->table_set_column_index(2);
										if (sc.loaded)
											ImGui::TextColored(ImVec4(0.35f, 0.75f, 0.35f, 1.f), "Loaded");
										else
											ImGui::Text("Dormant");
									}
									gui->end_table();
								}
								gui->pop_style_color(3);

								if (selected_script_idx >= 0 && selected_script_idx < (int)scripts::manager::script_list.size())
								{
									auto& sc = scripts::manager::script_list[selected_script_idx];

									ImGui::Dummy(ImVec2(0, 6.f));
									ImGui::Separator();
									ImGui::Dummy(ImVec2(0, 4.f));

									if (gui->button(sc.loaded ? "Unload Script" : "Load Script")) {
										if (sc.loaded)
											scripts::manager::unload(sc);
										else
											scripts::manager::load(sc);
									}

									if (gui->button("Delete Script File")) {
										scripts::manager::unload(sc);

										std::wstring wpath;
										int size_needed = MultiByteToWideChar(CP_UTF8, 0, sc.path.c_str(), (int)sc.path.size(), nullptr, 0);
										wpath.resize(size_needed);
										MultiByteToWideChar(CP_UTF8, 0, sc.path.c_str(), (int)sc.path.size(), &wpath[0], size_needed);

										DeleteFileW(wpath.c_str());

										scripts::manager::refresh_list();
										selected_script_idx = -1;
										memset(edit_buffer, 0, sizeof(edit_buffer));
									}

									if (!sc.last_error.empty()) {
										ImGui::Dummy(ImVec2(0, 4.f));
										ImGui::TextWrapped("Error: %s", sc.last_error.c_str());
									}
								}
							}
							gui->end_child();
						}
						gui->end_group();

						gui->sameline();

						gui->begin_group();
						{
							gui->begin_child("Script Editor", 2, 1);
							{
								gui->text_field("New Script Name", new_script_name, IM_ARRAYSIZE(new_script_name));

								if (gui->button("Create New Script")) {
									if (strlen(new_script_name) > 0) {
										std::string full_path = scripts::BASE_PATH + new_script_name + scripts::SCRIPTS_EXT;

										std::ofstream file(full_path, std::ios::binary);
										if (file.is_open()) {
											file << "-- New Delusive Script\n\nfunction draw_ui(tab, col)\n    \nend\n\nfunction draw()\n    \nend\n";
											file.close();
										}

										memset(new_script_name, 0, sizeof(new_script_name));
										scripts::manager::refresh_list();
										selected_script_idx = -1;
										memset(edit_buffer, 0, sizeof(edit_buffer));
									}
								}

								ImGui::Dummy(ImVec2(0, 4.f));
								ImGui::Separator();
								ImGui::Dummy(ImVec2(0, 4.f));

								if (selected_script_idx >= 0 && selected_script_idx < (int)scripts::manager::script_list.size())
								{
									auto& sc = scripts::manager::script_list[selected_script_idx];

									ImGui::Text("Editing: %s%s", sc.name.c_str(), scripts::SCRIPTS_EXT);
									ImGui::Dummy(ImVec2(0, 2.f));

									ImGui::PushStyleColor(ImGuiCol_FrameBg, draw->get_clr(clr->window.background_two));
									ImGui::PushStyleColor(ImGuiCol_Border, draw->get_clr(clr->widgets.stroke_two));
									ImGui::PushStyleColor(ImGuiCol_Text, draw->get_clr(clr->widgets.text));

									ImGui::InputTextMultiline("##source_code", edit_buffer, IM_ARRAYSIZE(edit_buffer),
										ImVec2(GetContentRegionAvail().x - 1, GetContentRegionAvail().y - 32.f),
										ImGuiInputTextFlags_AllowTabInput);

									ImGui::PopStyleColor(3);
									ImGui::Dummy(ImVec2(0, 4.f));

									if (gui->button("Save Changes")) {
										sc.content = edit_buffer;
										scripts::manager::save_content(sc);

										sc.size_str = scripts::manager::format_size(sc.content.size());

										if (sc.loaded) {
											scripts::manager::load(sc);
										}
									}
								}
								else {
									ImGui::TextColored(clr->accent.Value, "Delusive Lua API v1.1");
									ImGui::Dummy(ImVec2(0, 4.f));
									ImGui::TextWrapped("Select a script from the left table to load, edit its source code, or delete it permanently.");
									ImGui::Dummy(ImVec2(0, 4.f));
									ImGui::TextWrapped("Supported callback hooks:\n- function draw()\n- function draw_ui(tab, col)");
								}
							}
							gui->end_child();
						}
						gui->end_group();

						ImGui::PopFont();
					}
					gui->end_content();
				}
				gui->end();
			}

			if (var->gui.current_section[3])
			{
				gui->set_next_window_size_constraints(ImVec2(400, 400), GetIO().DisplaySize);
				gui->begin("configs", nullptr, var->window.flags | lock_flag);
				{
					draw->window_decorations();

					gui->set_cursor_pos(elements->content.window_padding + ImVec2(0, var->window.titlebar + 1));
					gui->begin_content();
					{
						ImGui::PushFont(var->font.tahoma);

						static int         selected_cfg_idx = 0;
						static std::string cached_meta_name = "";
						static config_meta cached_meta = {};

						// Кламп индекса
						if (!config::manager::config_list.empty()) {
							if (selected_cfg_idx >= (int)config::manager::config_list.size())
								selected_cfg_idx = (int)config::manager::config_list.size() - 1;
						}
						else {
							selected_cfg_idx = 0;
						}

						gui->begin_group();
						{
							gui->begin_child("File Manager", 2, 1);
							{
								static char new_cfg_name[32] = "";
								gui->text_field("New Config Name", new_cfg_name, IM_ARRAYSIZE(new_cfg_name));

								if (gui->button("Create & Save")) {
									std::string new_name(new_cfg_name);
									new_name.erase(0, new_name.find_first_not_of(" \t"));
									if (!new_name.empty())
										new_name.erase(new_name.find_last_not_of(" \t") + 1);

									if (new_name.empty()) {
										notifications::push("Config", "Enter a config name first.", notifications::WARNING);
									}
									else if (new_name.size() > 32) {
										notifications::push("Config", "Name too long (max 32 chars).", notifications::WARNING);
									}
									else if (new_name.find_first_of("\\/:*?\"<>|") != std::string::npos) {
										notifications::push("Config", "Invalid characters in name.", notifications::WARNING);
									}
									else if (std::find(config::manager::config_list.begin(),
										config::manager::config_list.end(), new_name)
										!= config::manager::config_list.end())
									{
										notifications::push("Config", ("Already exists: " + new_name).c_str(), notifications::WARNING);
									}
									else {
										config::manager::save(new_name);
										memset(new_cfg_name, 0, sizeof(new_cfg_name));

										auto it = std::find(config::manager::config_list.begin(),
											config::manager::config_list.end(), new_name);
										if (it != config::manager::config_list.end())
											selected_cfg_idx = (int)std::distance(config::manager::config_list.begin(), it);

										notifications::push("Config", ("Created: " + new_name).c_str(), notifications::SUCCESS);
									}
								}

								ImGui::Dummy(ImVec2(0, 5.f));
								ImGui::Separator();
								ImGui::Dummy(ImVec2(0, 5.f));

								if (!config::manager::config_list.empty())
								{
									std::vector<const char*> dropdown_items;
									for (const auto& cfg_name : config::manager::config_list)
										dropdown_items.push_back(cfg_name.c_str());

									gui->dropdown("Select Config", &selected_cfg_idx, dropdown_items.data(), (int)dropdown_items.size());

									const std::string selected_name = config::manager::config_list[selected_cfg_idx];

									if (selected_name != cached_meta_name) {
										cached_meta = config::manager::get_meta(selected_name);
										cached_meta_name = selected_name;
									}

									ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.f), "Author: %s",
										cached_meta.author.empty() ? "Unknown" : cached_meta.author.c_str());
									ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.f), "Created: %s",
										cached_meta.created.empty() ? "N/A" : cached_meta.created.c_str());

									bool is_default = (config::globals::settings_default_config == selected_name);
									ImGui::TextColored(
										is_default ? ImVec4(0.35f, 0.75f, 0.35f, 1.f) : ImVec4(0.6f, 0.6f, 0.6f, 1.f),
										"Status: %s", is_default ? "Default Configuration" : "Inactive at startup");

									ImGui::Dummy(ImVec2(0, 4.f));

									if (gui->button("Load Config")) {
										config::manager::load(selected_name);
										cached_meta_name = "";
										notifications::push("Config", ("Loaded: " + selected_name).c_str(), notifications::SUCCESS);
									}

									if (gui->button("Overwrite Selected")) {
										std::string name = selected_name;
										config::manager::save(name);

										auto it = std::find(config::manager::config_list.begin(),
											config::manager::config_list.end(), name);
										if (it != config::manager::config_list.end())
											selected_cfg_idx = (int)std::distance(config::manager::config_list.begin(), it);
										else
											selected_cfg_idx = 0;

										cached_meta_name = ""; // сбрасываем кеш мета
										notifications::push("Config", ("Saved: " + name).c_str(), notifications::SUCCESS);
									}

									if (gui->button("Set as Default")) {
										config::globals::settings_default_config = selected_name;
										config::manager::save_settings();
										notifications::push("Config", ("Default set: " + selected_name).c_str(), notifications::INFO);
									}

									const bool protected_cfg = config::manager::is_protected(selected_name);
									if (protected_cfg)
										ImGui::BeginDisabled();

									if (gui->button("Delete Config")) {
										std::string name = selected_name;
										config::manager::delete_config(name);
										selected_cfg_idx = 0;
										cached_meta_name = "";
										notifications::push("Config", ("Deleted: " + name).c_str(), notifications::WARNING);
									}

									if (protected_cfg)
										ImGui::EndDisabled();
								}
								else {
									ImGui::TextColored(ImVec4(0.75f, 0.25f, 0.25f, 1.f), "No configurations found.");
								}
							}
							gui->end_child();
						}
						gui->end_group();

						gui->sameline();

						gui->begin_group();
						{
							gui->begin_child("Data Exchange", 2, 1);
							{
								ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.f), "Quick Share via Clipboard");

								if (gui->button("Export to Clipboard")) {
									if (!config::manager::config_list.empty()) {
										const std::string selected_name = config::manager::config_list[selected_cfg_idx];
										std::string code = config::manager::export_config_full(selected_name);

										if (!code.empty()) {
											if (OpenClipboard(NULL)) {
												EmptyClipboard();
												HGLOBAL hGlob = GlobalAlloc(GMEM_FIXED, code.size() + 1);
												if (hGlob) {
													memcpy(hGlob, code.c_str(), code.size() + 1);
													SetClipboardData(CF_TEXT, hGlob);
												}
												CloseClipboard();
												notifications::push("Config", ("Exported: " + selected_name).c_str(), notifications::SUCCESS);
											}
										}
										else {
											notifications::push("Config", "Export failed: empty config.", notifications::ERROR_T);
										}
									}
									else {
										notifications::push("Config", "No config selected.", notifications::WARNING);
									}
								}

								if (gui->button("Import from Clipboard")) {
									std::string code_str;

									if (OpenClipboard(NULL)) {
										HANDLE hData = GetClipboardData(CF_TEXT);
										if (hData) {
											char* pText = static_cast<char*>(GlobalLock(hData));
											if (pText) { code_str = pText; GlobalUnlock(hData); }
										}
										CloseClipboard();
									}

									if (code_str.empty()) {
										notifications::push("Config", "Clipboard is empty.", notifications::ERROR_T);
									}
									else if (code_str.size() < 8) {
										notifications::push("Config", "Clipboard data too short.", notifications::ERROR_T);
									}
									else if (code_str.find("XOR") != std::string::npos) {
										notifications::push("Config", "Invalid clipboard data.", notifications::ERROR_T);
									}
									else if (config::manager::import_config_full(code_str)) {
										auto parts = rconfig_detail::split(code_str, "|");
										if (parts.size() >= 2) {
											const std::string& imported_name = parts[1];
											auto it = std::find(config::manager::config_list.begin(),
												config::manager::config_list.end(), imported_name);
											if (it != config::manager::config_list.end())
												selected_cfg_idx = (int)std::distance(config::manager::config_list.begin(), it);
										}
										cached_meta_name = "";
										notifications::push("Config", "Config imported successfully.", notifications::SUCCESS);
									}
									else {
										notifications::push("Config", "Import failed: invalid data.", notifications::ERROR_T);
									}
								}

								ImGui::Dummy(ImVec2(0, 5.f));
								ImGui::Separator();

								gui->checkbox("Autosave Enabled", &config::manager::autosave_enabled);
							}
							gui->end_child();
						}
						gui->end_group();

						ImGui::PopFont();
					}
					gui->end_content();
				}
				gui->end();
			}

			if (var->gui.current_section[4])
			{
				gui->set_next_window_size_constraints(ImVec2(400, 400), GetIO().DisplaySize);
				gui->begin("profile", nullptr, var->window.flags | lock_flag);
				{
					draw->window_decorations();

					gui->set_cursor_pos(elements->content.window_padding + ImVec2(0, var->window.titlebar + 1));
					gui->begin_content();
					{
						static bool  show_untrusted_confirm = false;
						static float untrusted_confirm_timer = 0.f;
						static bool  untrusted_confirm_ready = false;

						static bool  show_unload_confirm = false;
						static float unload_confirm_timer = 0.f;
						static bool  unload_confirm_ready = false;

						static const float dpi_values[] = { 1.0f, 1.25f, 1.5f, 1.75f, 2.0f };

						gui->begin_group();
						{
							const float h_one_third = (ImGui::GetWindowHeight() - elements->content.padding.y * 2.f - elements->content.spacing.y * 2.f) / 3.f;
							const float h_two_thirds = h_one_third * 2.f + elements->content.spacing.y;

							gui->begin_child("Account", 2, 0, ImVec2(0, h_two_thirds));
							{
								ImDrawList* draw_list = GetWindowDrawList();
								ImVec2      cursor_pos = GetCursorScreenPos();

								std::string username = g_discord_username.empty() ? "Connecting..." : g_discord_username;
								std::string discord_id = g_discord_id.empty() ? "0" : g_discord_id;
								std::string sub_type = "Lifetime";
								int         days_left = 999;

								const float avatar_size = 50.f;
								ImVec2 av_min = cursor_pos + ImVec2(10, 10);
								ImVec2 av_max = av_min + ImVec2(avatar_size, avatar_size);

								draw->rect_filled(draw_list, av_min, av_max, draw->get_clr(clr->window.background_two));
								draw->rect(draw_list, av_min, av_max, draw->get_clr(clr->window.stroke));

								if (g_discord_avatar)
								{
									ImU32 avatar_col = ImGui::ColorConvertFloat4ToU32(
										ImVec4(1.f, 1.f, 1.f, var->gui.menu_alpha));
									draw_list->AddImageRounded((ImTextureID)g_discord_avatar,
										av_min, av_max, ImVec2(0, 1), ImVec2(1, 0), avatar_col, 4.f);
								}
								else
								{
									draw->text_outline(draw_list, var->font.tahoma, var->font.tahoma->FontSize + 2.f,
										av_min + ImVec2(avatar_size / 2.f - 4.f, avatar_size / 2.f - 6.f),
										draw->get_clr(clr->widgets.text_inactive, var->gui.menu_alpha), "?");
								}

								ImVec2 text_pos = av_min + ImVec2(avatar_size + 12.f, -2.f);

								draw->text_outline(draw_list, var->font.tahoma, var->font.tahoma->FontSize,
									text_pos, draw->get_clr(clr->widgets.text), username.c_str());
								draw->text_outline(draw_list, var->font.tahoma, var->font.tahoma->FontSize,
									text_pos + ImVec2(0, 14), draw->get_clr(clr->widgets.text_inactive),
									("id: " + discord_id).c_str());
								draw->text_outline(draw_list, var->font.tahoma, var->font.tahoma->FontSize,
									text_pos + ImVec2(0, 30), draw->get_clr(clr->widgets.text_inactive), "sub: ");

								float sub_w = var->font.tahoma->CalcTextSizeA(var->font.tahoma->FontSize, FLT_MAX, 0.f, "sub: ").x;
								draw->text_outline(draw_list, var->font.tahoma, var->font.tahoma->FontSize,
									text_pos + ImVec2(sub_w, 30), draw->get_clr(clr->accent), sub_type.c_str());
								draw->text_outline(draw_list, var->font.tahoma, var->font.tahoma->FontSize,
									text_pos + ImVec2(0, 44), draw->get_clr(clr->widgets.text_inactive),
									("expires in: " + std::to_string(days_left) + " days").c_str());

								ImGui::Dummy(ImVec2(0, avatar_size + 15.f));

								ImVec2 line_start = GetCursorScreenPos() + ImVec2(10, 0);
								ImVec2 line_end = line_start + ImVec2(GetWindowWidth() / 2.f - 40.f, 0);
								draw->line(draw_list, line_start, line_end, draw->get_clr(clr->window.stroke), 1.f);

								ImGui::Dummy(ImVec2(0, 10.f));

								gui->set_cursor_pos(GetCursorPos() + ImVec2(10, 0));
								ImVec2 sys = GetCursorScreenPos();

								auto draw_kv = [&](ImVec2 pos, const char* key, const char* val, ImColor val_clr) {
									draw->text_outline(draw_list, var->font.tahoma, var->font.tahoma->FontSize,
										pos, draw->get_clr(clr->widgets.text_inactive), key);
									float kw = var->font.tahoma->CalcTextSizeA(var->font.tahoma->FontSize, FLT_MAX, 0.f, key).x;
									draw->text_outline(draw_list, var->font.tahoma, var->font.tahoma->FontSize,
										pos + ImVec2(kw, 0), draw->get_clr(val_clr), val);
									};

								draw_kv(sys, "Cheat status: ", "Undetected", ImColor(0.f, 1.f, 0.f, 1.f));
								draw_kv(sys + ImVec2(0, 16), "Build: ",
									(std::string(BUILD_VERSION) + "  |  " + BUILD_DATE).c_str(), clr->widgets.text);
								draw_kv(sys + ImVec2(0, 32), "Branch: ", "stable", clr->accent);

								ImGui::Dummy(ImVec2(0, 48.f));
							}
							gui->end_child();

							// ── Unload ─────────────────────────────────────────────
							gui->begin_child("Unload", 2, 0, ImVec2(0, h_one_third));
							{
								gui->label_keybind("Unload Key", &config::menu::unload_key.value, 0);


								if (gui->button("Unload Cheat")) {
									show_unload_confirm = true;
									unload_confirm_timer = 3.5f;
									unload_confirm_ready = false;
								}
							}
							gui->end_child();
						}
						gui->end_group();

						gui->sameline();

						gui->begin_group();
						{
							// ── Theme ──────────────────────────────────────────────
							gui->begin_child("Theme", 2, 3);
							{
								float col[4] = {
									config::menu::accent_color.value.Value.x,
									config::menu::accent_color.value.Value.y,
									config::menu::accent_color.value.Value.z,
									config::menu::accent_color.value.Value.w
								};
								if (gui->label_color_edit("Menu Accent", col, false)) {
									config::menu::accent_color.value = ImColor(col[0], col[1], col[2],
										std::clamp(col[3], 0.f, 1.f));
									// dirty флаг поднимется автоматически через set<>
								}
							}
							gui->end_child();

							// ── Style ──────────────────────────────────────────────
							gui->begin_child("Style", 2, 3);
							{
								if (gui->checkbox("Hover Highlight", &config::menu::hover_highlight.value)) {
									config::manager::save_settings();
								}
								if (gui->checkbox("Window Glow", &config::menu::shadow_enabled.value)) {
									config::manager::save_settings();
								}
								if (config::menu::shadow_enabled.value) {
									if (gui->slider_float("Glow Thickness", &config::menu::shadow_size.value, 1.f, 100.f)) {
										config::manager::save_settings();
									}
									if (gui->slider_float("Glow Alpha", &config::menu::shadow_alpha.value, 0.f, 1.f)) {
										config::manager::save_settings();
									}
								}

								static const char* dpi_items[] = { "75%", "100%", "125%", "150%", "175%", "200%" };
								if (gui->dropdown("DPI Scale", &config::menu::dpi_scale_idx.value,
									dpi_items, IM_ARRAYSIZE(dpi_items))) {
									config::manager::save_settings();
								}

								if (gui->checkbox("Lock Menu Layout", &config::menu::lock_layout.value)) {
									config::manager::save_settings();
								}
							}
							gui->end_child();

							// ── Interface ──────────────────────────────────────────
							gui->begin_child("Interface", 2, 3);
							{
								if (config::menu::overlay.value.size() < 5) {
									config::menu::overlay.value.resize(5, 0);
								}
								const char* overlay_names[5] = { "Watermark", "Keybinds", "Reload Indicator", "Flyhack Indicator", "Traps Indicator" };
								gui->multi_dropdown("Overlay",
									config::menu::overlay.value, overlay_names, 5);

								//ImGui::Dummy(ImVec2(0, 2.f));

								gui->label_keybind("Menu Key", &config::menu::menu_key.value, 0);

								//ImGui::Dummy(ImVec2(0, 2.f));

								// Untrusted Features
								bool& uf = config::menu::untrusted_enabled.value;
								if (uf) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f, 0.35f, 0.35f, 1.f));

								if (gui->button(uf ? "Untrusted Features: ON" : "Untrusted Features")) {
									if (!uf) {
										show_untrusted_confirm = true;
										untrusted_confirm_timer = 3.5f;
										untrusted_confirm_ready = false;
									}
									else {
										uf = false;
									}
								}

								if (uf) ImGui::PopStyleColor();

								if (ImGui::IsItemHovered()) {
									ImGui::BeginTooltip();
									ImGui::PushStyleColor(ImGuiCol_Text,
										uf ? ImVec4(0.85f, 0.35f, 0.35f, 1.f)
										: ImVec4(0.6f, 0.6f, 0.6f, 1.f));
									ImGui::Text(uf ? "Untrusted features ENABLED — use at own risk"
										: "Enable experimental / unsupported features");
									ImGui::PopStyleColor();
									ImGui::EndTooltip();
								}
							}
							gui->end_child();
						}
						gui->end_group();

						// ── Untrusted confirm dialog ───────────────────────────────
						if (show_untrusted_confirm)
						{
							ImDrawList* fg = ImGui::GetForegroundDrawList();
							ImGuiIO& io = ImGui::GetIO();

							const float fsz = var->font.tahoma->FontSize;
							const float pad = 10.f, w = 280.f;
							const float bar_h = fsz + pad * 2.f, line_h = fsz + 4.f;
							const float prog_h = 4.f, btn_h = fsz + 10.f;
							const float body_h = pad + line_h * 3.f + pad * 2.f + prog_h + pad * 2.f + btn_h + pad;
							const float total_h = bar_h + body_h;

							auto gc = [&](ImColor c, float a = -1.f) -> ImU32 {
								return draw->get_clr(c, a < 0.f ? c.Value.w : a);
								};

							const ImVec2 box_min = ImVec2(
								floorf((io.DisplaySize.x - w) * 0.5f),
								floorf((io.DisplaySize.y - total_h) * 0.5f));
							const ImVec2 box_max = box_min + ImVec2(w, total_h);

							const bool box_hov =
								io.MousePos.x >= box_min.x && io.MousePos.x <= box_max.x &&
								io.MousePos.y >= box_min.y && io.MousePos.y <= box_max.y;

							fg->AddRectFilled({ 0,0 }, io.DisplaySize, IM_COL32(0, 0, 0, 140));
							draw->rect_filled(fg, box_min, box_max, gc(clr->window.background_one));
							fg->AddLine(box_min + ImVec2(1, 0), { box_max.x - 1, box_min.y }, gc(clr->accent), 1.f);
							fg->AddLine(box_min + ImVec2(1, 1), { box_max.x - 1, box_min.y + 1 }, gc(clr->accent, 0.35f), 1.f);
							draw->rect(fg, box_min, box_max, gc(box_hov ? clr->accent : clr->window.stroke));

							const char* title = "Enable Untrusted Features?";
							float title_w = var->font.tahoma->CalcTextSizeA(fsz, FLT_MAX, 0.f, title).x;
							draw->text_outline(fg, var->font.tahoma, fsz,
								{ floorf(box_min.x + (w - title_w) * 0.5f), floorf(box_min.y + pad) },
								gc(clr->accent), title);
							fg->AddLine({ box_min.x + 1, box_min.y + bar_h }, { box_max.x - 1, box_min.y + bar_h },
								gc(clr->window.stroke), 1.f);

							float cy = box_min.y + bar_h + pad;
							const float cx = box_min.x + pad, content_w = w - pad * 2.f;

							const char* lines[] = { "This enables experimental features:",
													  "May cause crashes or bans.",
													  "Use only if you know what you're doing." };
							const ImU32  lclrs[] = { gc(clr->widgets.text),
													  gc(ImColor(0.85f,0.35f,0.35f,1.f)),
													  gc(clr->widgets.text_inactive) };
							for (int i = 0; i < 3; i++) {
								float lw = var->font.tahoma->CalcTextSizeA(fsz, FLT_MAX, 0.f, lines[i]).x;
								draw->text_outline(fg, var->font.tahoma, fsz,
									{ floorf(box_min.x + (w - lw) * 0.5f), floorf(cy) }, lclrs[i], lines[i]);
								cy += line_h;
							}
							cy += pad;

							if (untrusted_confirm_timer > 0.f) {
								untrusted_confirm_timer -= io.DeltaTime;
								if (untrusted_confirm_timer <= 0.f) { untrusted_confirm_timer = 0.f; untrusted_confirm_ready = true; }
							}
							const float frac = untrusted_confirm_ready ? 1.f : (1.f - untrusted_confirm_timer / 10.f);
							fg->AddRectFilled({ cx,cy }, { cx + content_w,cy + prog_h }, gc(clr->window.background_two), 2.f);
							fg->AddRectFilled({ cx,cy }, { floorf(cx + content_w * frac),cy + prog_h },
								gc(untrusted_confirm_ready ? clr->accent : clr->widgets.text_inactive, 0.8f), 2.f);
							cy += prog_h + 4.f;

							char tstr[32];
							if (!untrusted_confirm_ready) ImFormatString(tstr, sizeof(tstr), "%.0f sec", ceilf(untrusted_confirm_timer));
							else                          ImFormatString(tstr, sizeof(tstr), "Ready");
							float tw = var->font.tahoma->CalcTextSizeA(fsz, FLT_MAX, 0.f, tstr).x;
							draw->text_outline(fg, var->font.tahoma, fsz,
								{ floorf(box_min.x + (w - tw) * 0.5f), floorf(cy) },
								gc(untrusted_confirm_ready ? clr->accent : clr->widgets.text_inactive), tstr);
							cy += line_h + pad;

							const float gap = 6.f, btn_w = (content_w - gap) * 0.5f;

							auto make_btn = [&](float x, const char* lbl, bool ready, bool danger) -> bool {
								const ImVec2 bmin = { floorf(x), floorf(cy) };
								const ImVec2 bmax = { floorf(x + btn_w), floorf(cy + btn_h) };
								const bool hov = ready &&
									io.MousePos.x >= bmin.x && io.MousePos.x <= bmax.x &&
									io.MousePos.y >= bmin.y && io.MousePos.y <= bmax.y;
								const float a = ready ? 1.f : 0.3f;
								fg->AddRectFilled(bmin, bmax, gc(hov ? (danger ? clr->accent : clr->widgets.stroke_two) : clr->window.background_two, a), 3.f);
								draw->rect(fg, bmin, bmax, gc(ready ? (danger ? clr->accent : clr->window.stroke) : clr->window.stroke, a));
								float lw = var->font.tahoma->CalcTextSizeA(fsz, FLT_MAX, 0.f, lbl).x;
								draw->text_outline(fg, var->font.tahoma, fsz,
									{ floorf(bmin.x + (btn_w - lw) * 0.5f),floorf(bmin.y + (btn_h - fsz) * 0.5f) },
									gc(clr->widgets.text, a), lbl);
								return hov && io.MouseClicked[0];
								};

							if (make_btn(cx, "Enable", untrusted_confirm_ready, true)) {
								config::menu::untrusted_enabled.value = true;
								show_untrusted_confirm = false;
							}
							if (make_btn(cx + btn_w + gap, "Cancel", true, false)) {
								show_untrusted_confirm = false;
							}
						}

						// ── Unload confirm dialog ─────────────────────────────────
						if (show_unload_confirm)
						{
							ImDrawList* fg = ImGui::GetForegroundDrawList();
							ImGuiIO& io = ImGui::GetIO();

							const float fsz = var->font.tahoma->FontSize;
							const float pad = 10.f, w = 280.f;
							const float bar_h = fsz + pad * 2.f, line_h = fsz + 4.f;
							const float prog_h = 4.f, btn_h = fsz + 10.f;
							const float body_h = pad + line_h * 2.f + pad * 2.f + prog_h + pad * 2.f + btn_h + pad;
							const float total_h = bar_h + body_h;

							auto gc2 = [&](ImColor c, float a = -1.f) -> ImU32 {
								return draw->get_clr(c, a < 0.f ? c.Value.w : a);
								};

							const ImVec2 box_min = ImVec2(
								floorf((io.DisplaySize.x - w) * 0.5f),
								floorf((io.DisplaySize.y - total_h) * 0.5f));
							const ImVec2 box_max = box_min + ImVec2(w, total_h);

							const bool box_hov2 =
								io.MousePos.x >= box_min.x && io.MousePos.x <= box_max.x &&
								io.MousePos.y >= box_min.y && io.MousePos.y <= box_max.y;

							fg->AddRectFilled({ 0,0 }, io.DisplaySize, IM_COL32(0, 0, 0, 140));
							draw->rect_filled(fg, box_min, box_max, gc2(clr->window.background_one));
							fg->AddLine(box_min + ImVec2(1, 0), { box_max.x - 1, box_min.y }, gc2(ImColor(0.85f, 0.35f, 0.35f, 1.f)), 1.f);
							fg->AddLine(box_min + ImVec2(1, 1), { box_max.x - 1, box_min.y + 1 }, gc2(ImColor(0.85f, 0.35f, 0.35f, 0.35f)), 1.f);
							draw->rect(fg, box_min, box_max, gc2(box_hov2 ? ImColor(0.85f, 0.35f, 0.35f, 1.f) : clr->window.stroke));

							const char* title2 = "Unload Cheat?";
							float title2_w = var->font.tahoma->CalcTextSizeA(fsz, FLT_MAX, 0.f, title2).x;
							draw->text_outline(fg, var->font.tahoma, fsz,
								{ floorf(box_min.x + (w - title2_w) * 0.5f), floorf(box_min.y + pad) },
								gc2(ImColor(0.85f, 0.35f, 0.35f, 1.f)), title2);
							fg->AddLine({ box_min.x + 1, box_min.y + bar_h }, { box_max.x - 1, box_min.y + bar_h },
								gc2(clr->window.stroke), 1.f);

							float cy2 = box_min.y + bar_h + pad;
							const float cx2 = box_min.x + pad, content_w2 = w - pad * 2.f;

							const char* lines2[] = { "This will immediately eject the cheat.",
													   "All features will be disabled." };
							const ImU32 lclrs2[] = { gc2(clr->widgets.text),
													   gc2(clr->widgets.text_inactive) };
							for (int i = 0; i < 2; i++) {
								float lw = var->font.tahoma->CalcTextSizeA(fsz, FLT_MAX, 0.f, lines2[i]).x;
								draw->text_outline(fg, var->font.tahoma, fsz,
									{ floorf(box_min.x + (w - lw) * 0.5f), floorf(cy2) }, lclrs2[i], lines2[i]);
								cy2 += line_h;
							}
							cy2 += pad;

							if (unload_confirm_timer > 0.f) {
								unload_confirm_timer -= io.DeltaTime;
								if (unload_confirm_timer <= 0.f) { unload_confirm_timer = 0.f; unload_confirm_ready = true; }
							}
							const float frac2 = unload_confirm_ready ? 1.f : (1.f - unload_confirm_timer / 3.5f);
							fg->AddRectFilled({ cx2,cy2 }, { cx2 + content_w2,cy2 + prog_h }, gc2(clr->window.background_two), 2.f);
							fg->AddRectFilled({ cx2,cy2 }, { floorf(cx2 + content_w2 * frac2),cy2 + prog_h },
								gc2(unload_confirm_ready ? ImColor(0.85f, 0.35f, 0.35f, 1.f) : clr->widgets.text_inactive, 0.8f), 2.f);
							cy2 += prog_h + 4.f;

							char tstr2[32];
							if (!unload_confirm_ready) ImFormatString(tstr2, sizeof(tstr2), "%.0f sec", ceilf(unload_confirm_timer));
							else                       ImFormatString(tstr2, sizeof(tstr2), "Ready");
							float tw2 = var->font.tahoma->CalcTextSizeA(fsz, FLT_MAX, 0.f, tstr2).x;
							draw->text_outline(fg, var->font.tahoma, fsz,
								{ floorf(box_min.x + (w - tw2) * 0.5f), floorf(cy2) },
								gc2(unload_confirm_ready ? ImColor(0.85f, 0.35f, 0.35f, 1.f) : clr->widgets.text_inactive), tstr2);
							cy2 += line_h + pad;

							const float gap2 = 6.f, btn_w2 = (content_w2 - gap2) * 0.5f;

							auto make_btn2 = [&](float x, const char* lbl, bool ready, bool danger) -> bool {
								const ImVec2 bmin = { floorf(x), floorf(cy2) };
								const ImVec2 bmax = { floorf(x + btn_w2), floorf(cy2 + btn_h) };
								const bool hov = ready &&
									io.MousePos.x >= bmin.x && io.MousePos.x <= bmax.x &&
									io.MousePos.y >= bmin.y && io.MousePos.y <= bmax.y;
								const float a = ready ? 1.f : 0.3f;
								ImColor btn_fill = hov ? (danger ? ImColor(0.85f, 0.35f, 0.35f, 1.f) : clr->widgets.stroke_two) : clr->window.background_two;
								ImColor btn_border = ready ? (danger ? ImColor(0.85f, 0.35f, 0.35f, 1.f) : clr->window.stroke) : clr->window.stroke;
								fg->AddRectFilled(bmin, bmax, gc2(btn_fill, a), 3.f);
								draw->rect(fg, bmin, bmax, gc2(btn_border, a));
								float lw = var->font.tahoma->CalcTextSizeA(fsz, FLT_MAX, 0.f, lbl).x;
								draw->text_outline(fg, var->font.tahoma, fsz,
									{ floorf(bmin.x + (btn_w2 - lw) * 0.5f), floorf(bmin.y + (btn_h - fsz) * 0.5f) },
									gc2(clr->widgets.text, a), lbl);
								return hov && io.MouseClicked[0];
								};

							if (make_btn2(cx2, "Unload", unload_confirm_ready, true)) {
								show_unload_confirm = false;
								core::g_unloading = true;
							}
							if (make_btn2(cx2 + btn_w2 + gap2, "Cancel", true, false)) {
								show_unload_confirm = false;
							}
						}
					}
					gui->end_content();
				}
				gui->end();
			}		}

		var->window.width = GetCurrentWindow()->ContentSize.x + style->ItemSpacing.x;

		if (IsMouseHoveringRect(pos, pos + size))
			SetWindowFocus();
	}
	gui->end();
	gui->pop_style_var();
}

void c_gui::render_reload_indicator()
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

	auto gc = [&](ImColor color, float a = -1.f) -> ImU32 {
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

		draw->rect(GetBackgroundDrawList(), pos - ImVec2(1, 1), pos + size + ImVec2(1, 1), gc(ImColor(0.f, 0.f, 0.f, 0.5f)));
		draw->rect_filled(draw_list, pos, pos + size, gc(clr->window.background_one));
		draw->line(draw_list, pos + ImVec2(1, 1), pos + ImVec2(size.x - 1, 1), gc(clr->accent), 1);
		draw->line(draw_list, pos + ImVec2(1, 2), pos + ImVec2(size.x - 1, 2), gc(clr->accent, 0.4f), 1);
		draw->rect(draw_list, pos, pos + size, gc(clr->window.stroke));

		draw_list->PushClipRect(pos + ImVec2(1, 1), pos + size - ImVec2(1, 1), true);

		char text_buf[64];
		sprintf_s(text_buf, sizeof(text_buf), "reloading [%d%%]", (int)(progress * 100.f));
		ImVec2 text_size = var->font.tahoma->CalcTextSizeA(var->font.tahoma->FontSize, FLT_MAX, 0.f, text_buf);

		draw->text_outline(draw_list, var->font.tahoma, var->font.tahoma->FontSize,
			pos + ImVec2(std::floor(size.x / 2 - text_size.x / 2), 6.f),
			gc(clr->widgets.text), text_buf);

		float bar_pad_x = 10.f;
		float bar_h = 3.f;
		float bar_y = size.y - 8.f;

		ImU32 bar_bg = gc(clr->window.stroke);
		ImU32 bar_fill = gc(clr->accent);

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

void c_gui::render_flyhack_indicator()
{
	bool is_menu_open = var->gui.menu_opened;
	bool is_indicator_enabled = config::menu::overlay.value.size() > 3 && config::menu::overlay.value[3];
	bool is_on_server = g_is_on_server;

	// violation::tick() обновляет g_indicator_pct каждый фрейм [0..100]
	float progress = 0.0f;
	if (is_on_server) {
		progress = std::clamp(violation::g_indicator_pct / 100.f, 0.f, 1.f);
	}

	if (is_menu_open && progress <= 0.01f) {
		progress = std::fmodf((float)ImGui::GetTime() * 0.7f, 1.0f);
	}

	bool show_window = is_indicator_enabled && (is_menu_open || (is_on_server && progress > 0.01f));

	static float alpha = 0.f;
	alpha = ImClamp(alpha + (gui->fixed_speed(8.f) * (show_window ? 1.f : -1.f)), 0.f, 1.f);

	if (alpha <= 0.01f) return;

	// Плавный прогресс: быстро падает, медленно растёт
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
	const float H = 44.f;   // +6px под строку статуса

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

	// Динамический цвет: зелёный -> жёлтый -> оранжевый -> красный
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

	// Фон + рамка
	draw->rect_filled(dl, p, m, C(clr->window.background_one));
	draw->rect(dl, p, m, C(clr->window.stroke));

	// Верхняя линия — цвет зависит от прогресса
	ImU32 dyn_col      = bar_color(smooth_progress);
	ImU32 dyn_col_fade = bar_color(smooth_progress, 0.35f);
	draw->line(dl, p + ImVec2(1, 1), p + ImVec2(W - 1, 1), dyn_col,      1);
	draw->line(dl, p + ImVec2(1, 2), p + ImVec2(W - 1, 2), dyn_col_fade, 1);

	// Строка 1: "fly violation [XX%]"
	char text_buf[64];
	sprintf_s(text_buf, sizeof(text_buf), "fly violation [%d%%]", (int)(smooth_progress * 100.f));
	ImVec2 text_size = var->font.tahoma->CalcTextSizeA(var->font.tahoma->FontSize, FLT_MAX, 0.f, text_buf);
	draw->text_outline(dl, var->font.tahoma, var->font.tahoma->FontSize,
		ImVec2(p.x + (W - text_size.x) * 0.5f, p.y + 5.f),
		C(clr->widgets.text), text_buf);

	// Строка 2: статус с цветом прогресса
	const char* status_str;
	ImU32 status_col;
	if (smooth_progress < 0.3f) {
		status_str = "safe";
		status_col = bar_color(0.05f);
	} else if (smooth_progress < 0.6f) {
		status_str = "warning";
		status_col = bar_color(0.45f);
	} else if (smooth_progress < 0.82f) {
		status_str = "danger";
		status_col = bar_color(0.72f);
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

	// Прогресс-бар
	const float bar_pad_x = 8.f;
	const float bar_h     = 3.f;
	const float bar_y     = m.y - 8.f;

	ImU32 bar_bg   = C(clr->window.stroke);
	ImU32 bar_fill = bar_color(smooth_progress);

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

extern bool g_traps_active;

void c_gui::render_traps_indicator()
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

	auto bar_color = [&](float t, float a_mult = 1.f) -> ImU32 {
		return draw->get_clr(ImColor(1.0f - t, t, 0.0f, alpha * a_mult));
	};

	ImDrawList* dl = ImGui::GetBackgroundDrawList();
	const ImVec2 p = pos;
	const ImVec2 m = { p.x + W, p.y + H };

	draw->rect_filled(dl, p, m, C(clr->window.background_one));
	draw->rect(dl, p, m, C(clr->window.stroke));

	ImU32 dyn_col      = bar_color(smooth_progress);
	ImU32 dyn_col_fade = bar_color(smooth_progress, 0.35f);
	draw->line(dl, p + ImVec2(1, 1), p + ImVec2(W - 1, 1), dyn_col,      1);
	draw->line(dl, p + ImVec2(1, 2), p + ImVec2(W - 1, 2), dyn_col_fade, 1);

	char text_buf[64] = "traps exploit";
	ImVec2 text_size = var->font.tahoma->CalcTextSizeA(var->font.tahoma->FontSize, FLT_MAX, 0.f, text_buf);
	draw->text_outline(dl, var->font.tahoma, var->font.tahoma->FontSize,
		ImVec2(p.x + (W - text_size.x) * 0.5f, p.y + 5.f),
		C(clr->widgets.text), text_buf);

	const char* status_str = "inactive";
	ImU32 status_col = bar_color(0.0f);

	if (smooth_progress > 0.5f) {
		status_str = "active";
		status_col = bar_color(1.0f);
	}

	ImVec2 status_size = var->font.tahoma->CalcTextSizeA(var->font.tahoma->FontSize, FLT_MAX, 0.f, status_str);
	draw->text_outline(dl, var->font.tahoma, var->font.tahoma->FontSize,
		ImVec2(p.x + (W - status_size.x) * 0.5f,
			   p.y + 5.f + var->font.tahoma->FontSize + 3.f),
		status_col, status_str);

	const float bar_pad_x = 8.f;
	const float bar_h     = 3.f;
	const float bar_y     = m.y - 8.f;

	ImU32 bar_bg   = C(clr->window.stroke);
	ImU32 bar_fill = bar_color(smooth_progress);

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
