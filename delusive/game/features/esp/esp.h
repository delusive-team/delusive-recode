#pragma once
#include <algorithm>
#include <cmath>
#include "../../sdk/sdk.h"
#include "../../../configs/configs.h"
#include "../../../core/esp_renderer.h"
#include "../../../vcruntime/logger/logger.h"
#include "shared/shared.h"

inline std::vector< std::pair< enums::e_bone, enums::e_bone > > bones = {

	{ enums::e_bone::head, enums::e_bone::spine4 },
	{ enums::e_bone::spine4, enums::e_bone::l_upperarm },
	{ enums::e_bone::l_upperarm, enums::e_bone::l_forearm },

	{ enums::e_bone::l_forearm, enums::e_bone::l_hand },
	{ enums::e_bone::spine4, enums::e_bone::r_upperarm },
	{ enums::e_bone::r_upperarm, enums::e_bone::r_forearm },

	{ enums::e_bone::r_forearm, enums::e_bone::r_hand },
	{ enums::e_bone::spine4, enums::e_bone::pelvis },
	{ enums::e_bone::pelvis, enums::e_bone::l_hip },

	{ enums::e_bone::l_hip , enums::e_bone::l_knee },
	{ enums::e_bone::l_knee, enums::e_bone::l_foot },
	{ enums::e_bone::pelvis, enums::e_bone::r_hip },

	{ enums::e_bone::r_hip, enums::e_bone::r_knee },
	{ enums::e_bone::r_knee, enums::e_bone::r_foot },
	{ enums::e_bone::r_foot, enums::e_bone::r_toe },
	{ enums::e_bone::l_foot, enums::e_bone::l_toe }
};

namespace esp {
	struct c_esp_label {
		std::string text;
		ImU32 color;
		int align; // 1=top, 2=bot, 3=left, 4=right
		float font_size;
		int style;
		vec2_t size;
		float ox, oy;
	};

	struct c_label_manager {
		std::vector<c_esp_label> labels;

		void add(const std::string& text, ImU32 color, int align, float font_size, int style, float ox, float oy) {
			if (text.empty()) return;
			c_esp_label l;
			l.text = text; l.color = color; l.align = align; l.font_size = font_size; l.style = style;
			l.ox = ox; l.oy = oy;
			ImVec2 sz = esp_render.calc_text_size(text.c_str(), font_size);
			l.size = { sz.x, sz.y };
			labels.push_back(l);
		}

		void render(vec2_t box_pos, vec2_t box_size) {
			std::vector<c_esp_label*> grp[5];
			for (auto& l : labels) {
				int a = l.align;
				if (a < 1 || a > 4) a = 1; // fallback to top
				grp[a].push_back(&l);
			}

			auto get_flags = [&](int style, int align) -> uint32_t {
				uint32_t f = text_none;
				if (style == 1) f |= text_outline;
				else if (style == 2) f |= text_dropshadow;
				
				if (align == 1 || align == 2 || align == 0) f |= text_center_x;
				else if (align == 3) f |= text_right;
				return f;
			};

			float spacing_y = 2.f;

			if (!grp[3].empty()) { // Left
				float cy = box_pos.y;
				for (auto* l : grp[3]) {
					float px = box_pos.x - 4.f + l->ox;
					float py = cy + l->oy;
					esp_render.string(std::floor(px), std::floor(py), l->font_size, get_flags(l->style, 3), l->color, l->text.c_str());
					cy += l->size.y + spacing_y;
				}
			}
			if (!grp[4].empty()) { // Right
				float cy = box_pos.y;
				for (auto* l : grp[4]) {
					float px = box_pos.x + box_size.x + 4.f + l->ox;
					float py = cy + l->oy;
					esp_render.string(std::floor(px), std::floor(py), l->font_size, get_flags(l->style, 4), l->color, l->text.c_str());
					cy += l->size.y + spacing_y;
				}
			}
			if (!grp[1].empty()) { // Top
				float total_h = 0.f;
				for (auto* l : grp[1]) total_h += l->size.y + spacing_y;
				float cy = box_pos.y - total_h - 4.f;
				for (auto* l : grp[1]) {
					float px = box_pos.x + box_size.x * 0.5f + l->ox;
					float py = cy + l->oy;
					esp_render.string(std::floor(px), std::floor(py), l->font_size, get_flags(l->style, 1), l->color, l->text.c_str());
					cy += l->size.y + spacing_y;
				}
			}
			if (!grp[2].empty()) { // Bottom
				float cy = box_pos.y + box_size.y + 4.f + spacing_y;
				for (auto* l : grp[2]) {
					float px = box_pos.x + box_size.x * 0.5f + l->ox;
					float py = cy + l->oy;
					esp_render.string(std::floor(px), std::floor(py), l->font_size, get_flags(l->style, 2), l->color, l->text.c_str());
					cy += l->size.y + spacing_y;
				}
			}
		}
	};

	inline void draw_view_direction(sdk::BasePlayer* player, const unity::Camera::FrameCtx& ctx, ImU32 color) {
		if (!memory::is_valid(player->eyes())) return;
		vec3_t head_pos = player->bone_position(enums::e_bone::head);
		vec3_t forward = player->eyes()->head_forward();
		
		if (!player->is_sleeping() && !player->is_knocked()) {
			head_pos.y += 0.15f;
		}

		vec3_t end_pos = head_pos + (forward * 1.5f);
		
		vec2_t s_head, s_end;
		if (ctx.project(head_pos, s_head) && ctx.project(end_pos, s_end)) {
			esp_render.line(s_head.x, s_head.y, s_end.x, s_end.y, color, 1.5f);
		}
	}

	inline void draw_oof_arrow(const unity::Camera::FrameCtx& ctx, vec3_t target_pos, ImU32 color, bool rainbow, int style, float radius, bool pulse) {
		if (!sdk::local_player || !memory::is_valid(sdk::local_player->eyes())) return;
		
		vec3_t local_head = sdk::local_player->bone_position(enums::e_bone::head);
		vec3_t local_forward = sdk::local_player->eyes()->head_forward();

		vec3_t to_target = target_pos - local_head;
		float mag = std::sqrt(to_target.x * to_target.x + to_target.y * to_target.y + to_target.z * to_target.z);
		if (mag <= 0.001f) return;

		to_target.x /= mag; to_target.z /= mag;
		float angle_to = std::atan2(to_target.x, to_target.z);
		float angle_forward = std::atan2(local_forward.x, local_forward.z);
		float arrow_angle = angle_to - angle_forward;

		const float PI = 3.14159265f;
		const float TWO_PI = 6.2831853f;
		while (arrow_angle < -PI) arrow_angle += TWO_PI;
		while (arrow_angle > PI) arrow_angle -= TWO_PI;

		ImU32 oof_col = color;
		if (rainbow) {
			float t = std::fmodf((float)ImGui::GetTime() * 0.5f, 1.0f);
			ImVec4 rc; ImGui::ColorConvertHSVtoRGB(t, 0.85f, 1.0f, rc.x, rc.y, rc.z);
			rc.w = ((color >> 24) & 0xFF) / 255.f;
			oof_col = ImGui::ColorConvertFloat4ToU32(rc);
		}
		if (pulse) {
			float p_val = 0.5f + 0.5f * std::sin(ImGui::GetTime() * 5.0f);
			int a = (oof_col >> 24) & 0xFF;
			a = static_cast<int>(a * (0.3f + p_val * 0.7f));
			oof_col = (oof_col & 0x00FFFFFF) | (a << 24);
		}

		float sin_a = std::sin(arrow_angle);
		float cos_a = std::cos(arrow_angle);
		
		ImVec2 center = { ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f };
		float cX = center.x + sin_a * radius;
		float cY = center.y - cos_a * radius;

		ImDrawList* dl = ImGui::GetBackgroundDrawList();
		if (style == 0) {
			float size = 12.0f;
			ImVec2 tip = { cX + sin_a * size, cY - cos_a * size };
			ImVec2 l_pt = { cX + std::sin(arrow_angle - 2.4f) * size, cY - std::cos(arrow_angle - 2.4f) * size };
			ImVec2 r_pt = { cX + std::sin(arrow_angle + 2.4f) * size, cY - std::cos(arrow_angle + 2.4f) * size };
			dl->AddTriangleFilled(tip, l_pt, r_pt, oof_col);
		} else if (style == 1) {
			float size = 12.0f;
			ImVec2 tip = { cX + sin_a * size, cY - cos_a * size };
			ImVec2 l_pt = { cX + std::sin(arrow_angle - 2.3f) * size, cY - std::cos(arrow_angle - 2.3f) * size };
			ImVec2 r_pt = { cX + std::sin(arrow_angle + 2.3f) * size, cY - std::cos(arrow_angle + 2.3f) * size };
			ImVec2 inner_pt = { cX, cY };
			dl->AddLine(tip, l_pt, oof_col, 2.5f);
			dl->AddLine(tip, r_pt, oof_col, 2.5f);
			dl->AddLine(l_pt, inner_pt, oof_col, 2.5f);
			dl->AddLine(r_pt, inner_pt, oof_col, 2.5f);
		} else if (style == 2) {
			float arcWidth = 35.0f / radius;
			int segments = 12;
			float step = arcWidth / segments;
			float startAngle = arrow_angle - (arcWidth / 2.0f);
			dl->PathClear();
			for (int i = 0; i <= segments; i++) {
				float a = startAngle + (step * i);
				dl->PathLineTo({ center.x + std::sin(a) * radius, center.y - std::cos(a) * radius });
			}
			dl->PathStroke(oof_col, 0, 3.0f);
		}
	}

	inline void render_player(sdk::BasePlayer* player, const unity::Camera::FrameCtx& ctx) {
		if (!config::esp::players_show_teammates && player->is_teammate()) {
			static bool log1 = false; if (!log1) { LOG_WARN("[ESP] render_player: skipped teammate"); log1 = true; }
			return;
		}

		const auto& model = player->model();
		if (!memory::is_valid(model)) {
			static bool log2 = false; if (!log2) { LOG_WARN("[ESP] render_player: invalid model"); log2 = true; }
			return;
		}

		const auto& is_npc = model->is_npc();
		if ((is_npc && !config::esp::players_show_npcs)) {
			static bool log3 = false; if (!log3) { LOG_WARN("[ESP] render_player: skipped NPC"); log3 = true; }
			return;
		}

		const auto& is_sleeping = player->is_sleeping();
		if (is_sleeping && !config::esp::players_show_sleepers) {
			static bool log4 = false; if (!log4) { LOG_WARN("[ESP] render_player: skipped sleeper"); log4 = true; }
			return;
		}

		const auto& is_knocked = player->is_knocked();
		if (is_knocked && !config::esp::players_show_wounded) {
			return;
		}

		const auto& transform = player->transform();
		if (!memory::is_valid(transform)) {
			static bool log5 = false; if (!log5) { LOG_WARN("[ESP] render_player: invalid transform"); log5 = true; }
			return;
		}

		const auto& base_position = transform->position();

		const auto& distance = sdk::local_position.distance_to(base_position);
		if (distance > config::esp::players_max_render_distance) {
			static bool log6 = false; if (!log6) { LOG_WARN("[ESP] render_player: skipped due to distance"); log6 = true; }
			return;
		}

		const auto& is_visible = player->is_visible();
		if (config::esp::players_only_visible && !is_visible) {
			static bool log7 = false; if (!log7) { LOG_WARN("[ESP] render_player: skipped not visible"); log7 = true; }
			return;
		}

		vec2_t base_point, head_point, standing_point;
		
		if (!ctx.project(base_position, base_point) || 
			!ctx.project(player->bone_position(enums::e_bone::head), head_point) || 
			!ctx.project(base_position + vec3_t{ 0.f, 1.9f, 0.f }, standing_point)) {
			static bool log8 = false; if (!log8) { LOG_WARN("[ESP] render_player: w2s failed"); log8 = true; }
			return;
		}

		static bool log9 = false; if (!log9) { LOG_OK("[ESP] render_player: Drawing box!"); log9 = true; }

		float calc_w = std::abs(base_point.y - standing_point.y) * 0.45f;
		float calc_h = std::abs(base_point.y - head_point.y) * 1.15f;

		const auto box_size = vec2_t{ std::floor(calc_w), std::floor(calc_h) };
		const auto box_position = vec2_t{ std::floor(base_point.x - box_size.x * 0.5f), std::floor(base_point.y - box_size.y) };

		if (config::esp::players_box) {
			ImU32 box_color = (ImColor)config::esp::players_box_color;
			ImU32 fill_color = (ImColor)config::esp::players_box_fill_color;
			ImU32 outline_col = ImColor(0, 0, 0, 255);

			if (config::esp::players_box_fill) {
				esp_render.rect_filled(box_position.x, box_position.y, box_size.x, box_size.y, fill_color);
			}

			if (config::esp::players_box_type == 0) {
				// normal box with outline
				esp_render.box(box_position.x - 1, box_position.y - 1, box_size.x + 2, box_size.y + 2, outline_col);
				esp_render.box(box_position.x + 1, box_position.y + 1, box_size.x - 2, box_size.y - 2, outline_col);
				esp_render.box(box_position.x, box_position.y, box_size.x, box_size.y, box_color);
			} else {
				// corner box
				esp_render.corner_box(box_position.x - 1, box_position.y - 1, box_size.x + 2, box_size.y + 2, (box_size.x + 2) / 4.f, outline_col);
				esp_render.corner_box(box_position.x + 1, box_position.y + 1, box_size.x - 2, box_size.y - 2, (box_size.x - 2) / 4.f, outline_col);
				esp_render.corner_box(box_position.x, box_position.y, box_size.x, box_size.y, box_size.x / 4.f, box_color);
			}
		}

		if (config::esp::players_skeleton) {
			ImU32 skeleton_color = (ImColor)config::esp::players_skeleton_color;
			float thickness = config::esp::players_skeleton_thickness;
			bool draw_joints = config::esp::players_skeleton_joints;

			auto draw_bone = [&](enums::e_bone b1, enums::e_bone b2) {
				vec2_t p1, p2;
				if (ctx.project(player->bone_position(b1), p1) && ctx.project(player->bone_position(b2), p2)) {
					esp_render.line(p1.x, p1.y, p2.x, p2.y, skeleton_color, thickness);
					if (draw_joints) esp_render.circle_filled(p1.x, p1.y, thickness * 1.5f, skeleton_color, 12);
				}
			};

			for (const auto& bone_pair : bones) {
				draw_bone(bone_pair.first, bone_pair.second);
			}

			if (config::esp::players_skeleton_fingers && distance < 30.f) {
				draw_bone(enums::e_bone::r_hand, enums::e_bone::r_thumb1); draw_bone(enums::e_bone::r_thumb1, enums::e_bone::r_thumb2); draw_bone(enums::e_bone::r_thumb2, enums::e_bone::r_thumb3);
				draw_bone(enums::e_bone::r_hand, enums::e_bone::r_index1); draw_bone(enums::e_bone::r_index1, enums::e_bone::r_index2); draw_bone(enums::e_bone::r_index2, enums::e_bone::r_index3);
				draw_bone(enums::e_bone::r_hand, enums::e_bone::r_middle1); draw_bone(enums::e_bone::r_middle1, enums::e_bone::r_middle2); draw_bone(enums::e_bone::r_middle2, enums::e_bone::r_middle3);
				draw_bone(enums::e_bone::r_hand, enums::e_bone::r_ring1); draw_bone(enums::e_bone::r_ring1, enums::e_bone::r_ring2); draw_bone(enums::e_bone::r_ring2, enums::e_bone::r_ring3);
				
				draw_bone(enums::e_bone::l_hand, enums::e_bone::l_thumb1); draw_bone(enums::e_bone::l_thumb1, enums::e_bone::l_thumb2); draw_bone(enums::e_bone::l_thumb2, enums::e_bone::l_thumb3);
				draw_bone(enums::e_bone::l_hand, enums::e_bone::l_index1); draw_bone(enums::e_bone::l_index1, enums::e_bone::l_index2); draw_bone(enums::e_bone::l_index2, enums::e_bone::l_index3);
				draw_bone(enums::e_bone::l_hand, enums::e_bone::l_middle1); draw_bone(enums::e_bone::l_middle1, enums::e_bone::l_middle2); draw_bone(enums::e_bone::l_middle2, enums::e_bone::l_middle3);
				draw_bone(enums::e_bone::l_hand, enums::e_bone::l_ring1); draw_bone(enums::e_bone::l_ring1, enums::e_bone::l_ring2); draw_bone(enums::e_bone::l_ring2, enums::e_bone::l_ring3);
			}
		}

		if (config::esp::players_snapline) {
			ImU32 snap_col = (ImColor)config::esp::players_snapline_color;
			float sx = ImGui::GetIO().DisplaySize.x * 0.5f;
			float sy = 0.f;
			if (config::esp::players_snapline_origin == 1) sy = ImGui::GetIO().DisplaySize.y * 0.5f;
			else if (config::esp::players_snapline_origin == 2) sy = ImGui::GetIO().DisplaySize.y;
			
			float end_y = (config::esp::players_snapline_origin == 0) ? box_position.y : box_position.y + box_size.y;
			esp_render.line(sx, sy, box_position.x + box_size.x * 0.5f, end_y, ImColor(0, 0, 0, 255), 3.f);
			esp_render.line(sx, sy, box_position.x + box_size.x * 0.5f, end_y, snap_col, 1.f);
		}

		c_label_manager labels;

		auto format_text = [](std::string& str, int text_case) {
			if (text_case == 1) std::transform(str.begin(), str.end(), str.begin(), ::toupper);
			else if (text_case == 2) std::transform(str.begin(), str.end(), str.begin(), ::tolower);
		};

		if (config::esp::players_name) {
			const auto& player_name = player->display_name();
			if (player_name) {
				std::string name_str = player_name->string();
				format_text(name_str, config::esp::players_name_case);
				labels.add(name_str, (ImColor)config::esp::players_name_color, config::esp::prev_name_align, config::esp::prev_name_sz, config::esp::players_name_style, config::esp::prev_name_ox, config::esp::prev_name_oy);
			}
		}

		if (config::esp::players_distance) {
			std::string dist_str = "[" + std::to_string((int)distance) + "m]";
			format_text(dist_str, config::esp::players_distance_case);
			labels.add(dist_str, (ImColor)config::esp::players_distance_color, config::esp::prev_dist_align, config::esp::prev_dist_sz, config::esp::players_distance_style, config::esp::prev_dist_ox, config::esp::prev_dist_oy);
		}

		if (config::esp::players_health) {
			float health = player->health();
			float max_health = is_npc ? player->max_health() : 100.f;
			if (max_health <= 0.f) max_health = 100.f;
			float health_pc = std::clamp(health / max_health, 0.f, 1.f);
			
			float bar_h = std::floor(box_size.y * health_pc);
			float bar_y = std::floor(box_position.y + box_size.y - bar_h);
			
			ImU32 hp_color = (ImColor)config::esp::players_health_color;
			ImU32 bg_color = ImColor(0, 0, 0, 240);
			float thick = config::esp::prev_health_sz;
			
			float bar_x = std::floor(box_position.x - thick - 3.f + config::esp::prev_health_ox);
			float base_y = std::floor(box_position.y + config::esp::prev_health_oy);
			float hp_bar_y = std::floor(base_y + box_size.y - bar_h);
			
			esp_render.rect_filled(bar_x - 1.f, base_y - 1.f, thick + 2.f, std::floor(box_size.y + 2.f), bg_color);
			esp_render.rect_filled(bar_x, hp_bar_y, thick, bar_h, hp_color);

			if (config::esp::players_health_text) {
				char hp_str[16];
				snprintf(hp_str, sizeof(hp_str), "%d", (int)health);
				
				if (config::esp::players_health_text_pos == 0) {
					esp_render.string(std::floor(bar_x + thick * 0.5f), std::floor(hp_bar_y - 6.f), 10.f, text_center_x | text_outline, ImColor(255, 255, 255, 255), hp_str);
				} else {
					esp_render.string(std::floor(bar_x - 2.f), std::floor(hp_bar_y - 6.f), 10.f, text_right | text_outline, ImColor(255, 255, 255, 255), hp_str);
				}
			}
		}

		if (config::esp::players_weapon) {
			const auto& item = player->item();
			if (item) {
				const auto& info = item->info();
				if (memory::is_valid(info)) {
					const auto& shortname = info->shortname();
					if (memory::is_valid(shortname)) {
						std::string weapon_name = shortname->string();
						format_text(weapon_name, config::esp::players_weapon_case);
						labels.add(weapon_name, (ImColor)config::esp::players_weapon_color, config::esp::prev_weap_align, config::esp::prev_weap_sz, config::esp::players_weapon_style, config::esp::prev_weap_ox, config::esp::prev_weap_oy);
					}
				}
			}
		}

		if (config::esp::players_flags) {
			auto pos_to_align = [](int pos) -> int {
				if (pos == 0) return 1; // top
				if (pos == 1) return 2; // bot
				if (pos == 2) return 3; // left
				if (pos == 3) return 4; // right
				return 4;
			};

			auto add_flag = [&](bool cond, const char* text, float ox, float oy, float sz, int pos_idx, int style, ImU32 col) {
				if (!cond) return;
				labels.add(text, col, pos_to_align(pos_idx), sz, style, ox, oy);
			};

			add_flag(config::esp::players_flag_sleep && is_sleeping, "SLEEP", config::esp::prev_flag_sleep_ox, config::esp::prev_flag_sleep_oy, config::esp::prev_flag_sleep_sz, config::esp::players_flags_sleep_pos, config::esp::prev_flag_sleep_style, (ImColor)config::esp::prev_flag_sleep_color);
			add_flag(config::esp::players_flag_visible && is_visible, "VISIBLE", config::esp::prev_flag_visible_ox, config::esp::prev_flag_visible_oy, config::esp::prev_flag_visible_sz, config::esp::players_flags_visible_pos, config::esp::prev_flag_visible_style, (ImColor)config::esp::prev_flag_visible_color);
			add_flag(is_knocked, "WOUNDED", config::esp::prev_flag_inside_ox, config::esp::prev_flag_inside_oy, config::esp::prev_flag_inside_sz, config::esp::players_flags_inside_pos, config::esp::prev_flag_inside_style, (ImColor)config::esp::prev_flag_inside_color);
			add_flag(config::esp::players_flag_team && player->is_teammate(), "TEAM", config::esp::prev_flag_team_ox, config::esp::prev_flag_team_oy, config::esp::prev_flag_team_sz, config::esp::players_flags_team_pos, config::esp::prev_flag_team_style, (ImColor)config::esp::prev_flag_team_color);
			add_flag(config::esp::players_flag_zooming && player->has_flag(8192), "ZOOM", config::esp::prev_flag_zooming_ox, config::esp::prev_flag_zooming_oy, config::esp::prev_flag_zooming_sz, config::esp::players_flags_zooming_pos, config::esp::prev_flag_zooming_style, (ImColor)config::esp::prev_flag_zooming_color);
			add_flag(config::esp::players_flag_safezone && player->has_flag(65536), "SAFEZONE", config::esp::prev_flag_safezone_ox, config::esp::prev_flag_safezone_oy, config::esp::prev_flag_safezone_sz, config::esp::players_flags_safezone_pos, config::esp::prev_flag_safezone_style, (ImColor)config::esp::prev_flag_safezone_color);
			add_flag(config::esp::players_flag_mount && memory::is_valid(player->mounted()), "MOUNT", config::esp::prev_flag_mount_ox, config::esp::prev_flag_mount_oy, config::esp::prev_flag_mount_sz, config::esp::players_flags_mount_pos, config::esp::prev_flag_mount_style, (ImColor)config::esp::prev_flag_mount_color);
		}

		labels.render(box_position, box_size);
	}	

	inline void entities_loop() {
		static bool logged_success = false;
		static bool logged_client_err = false;
		static bool logged_entity_err = false;

		const auto& client_entities = sdk::BaseNetworkable::client_entities();
		if (!memory::is_valid(client_entities)) {
			if (!logged_client_err) {
				LOG_ERROR("[ESP] client_entities is invalid!");
				logged_client_err = true;
			}
			sdk::clear();
			return;
		}
		logged_client_err = false; // Reset when valid again

		const auto& entity_list = client_entities->entity_list();
		if (!memory::is_valid(entity_list)) {
			if (!logged_entity_err) {
				LOG_ERROR("[ESP] entity_list is invalid!");
				logged_entity_err = true;
			}
			sdk::clear();
			return;
		}
		logged_entity_err = false; // Reset when valid again

		const auto& count = entity_list->count();
		if (count <= 0) {
			sdk::clear();
			return;
		}

		if (!logged_success) {
			LOG_OK("[ESP] entity_list connected! Total entities: %d", count);
			logged_success = true;
		}

		{
			std::lock_guard<std::mutex> lock(sdk::info.mtx);
			sdk::info.players.clear();

			for (auto i = 0; i < count; ++i) {
				const auto base_networkable = entity_list->get< uintptr_t >(i);
				if (!memory::is_valid(base_networkable)) {
					static bool logged_inv_net = false;
					if (!logged_inv_net) {
						LOG_WARN("[ESP] Invalid base_networkable at index %d", i);
						logged_inv_net = true;
					}
					continue;
				}

				const auto& base_object = mem::read< uintptr_t >(base_networkable + 0x10);
				if (!memory::is_valid(base_object)) {
					static bool logged_inv_obj1 = false;
					if (!logged_inv_obj1) {
						LOG_WARN("[ESP] Invalid base_object at index %d", i);
						logged_inv_obj1 = true;
					}
					continue;
				}

				const auto& object = mem::read< uintptr_t >(base_object + 0x30);
				if (!memory::is_valid(object)) {
					static bool logged_inv_obj2 = false;
					if (!logged_inv_obj2) {
						LOG_WARN("[ESP] Invalid object at index %d", i);
						logged_inv_obj2 = true;
					}
					continue;
				}

				const auto& tag = mem::read< uint16_t >(object + 0x54);
				if (tag == 6) {
					static bool logged_tag6 = false;
					if (!logged_tag6) {
						LOG("[ESP] Found entity with tag 6 (Player)!");
						logged_tag6 = true;
					}

					const auto& player = mem::read< sdk::BasePlayer* >(base_object + 0x28);
					if (!memory::is_valid(player)) {
						static bool logged_inv_player = false;
						if (!logged_inv_player) {
							LOG_ERROR("[ESP] Player pointer is invalid for tag 6!");
							logged_inv_player = true;
						}
						continue;
					}

					if (player->lifestate() || player->is_local_player())
						continue;

					static bool logged_valid_player = false;
					if (!logged_valid_player) {
						LOG_OK("[ESP] Successfully parsed a valid player to ESP list!");
						logged_valid_player = true;
					}

					sdk::info.players.push_back(player);
				}
			}
		}

		if (!sdk::MapInterface::is_open()) {
			auto ctx = unity::Camera::get_frame_context();
			
			std::vector<sdk::BasePlayer*> players_copy;
			{
				std::lock_guard<std::mutex> lock(sdk::info.mtx);
				players_copy = sdk::info.players;
			}

			for (auto&& player : players_copy) {
				render_player(player, ctx);
			}

			// ── Shared ESP ──────────────────────────────────────────────────────────
			if (config::shared_esp::enabled.value && g_shared_esp.enabled.load()) {

				// ── 1. Push our data (own pos + visible enemy list) to server ───────
				if (sdk::local_player && memory::is_valid(sdk::local_player)) {
					sh_entity_t local_self{};
					local_self.userid  = sdk::local_player->user_id();
					local_self.is_peer = true;
					local_self.hp      = (int)sdk::local_player->health();
					auto lpos          = sdk::local_position;
					local_self.x = lpos.x; local_self.y = lpos.y; local_self.z = lpos.z;
					
					unity::c_string* local_name = sdk::local_player->display_name();
					if (local_name) {
						std::string lname = local_name->string();
						strncpy_s(local_self.name, lname.c_str(), _TRUNCATE);
					}

					// Build enemy list from current ESP scan
					std::vector<sh_entity_t> visible_enemies;
					for (auto& ep : players_copy) {
						if (!memory::is_valid(ep)) continue;
						const auto& tr = ep->transform();
						if (!memory::is_valid(tr)) continue;
						sh_entity_t e{};
						e.userid   = ep->user_id();
						e.hp       = (int)ep->health();
						e.is_peer  = false;
						auto epos  = tr->position();
						e.x = epos.x; e.y = epos.y; e.z = epos.z;
						
						unity::c_string* ename = ep->display_name();
						if (ename) {
							std::string name_str = ename->string();
							strncpy_s(e.name, name_str.c_str(), _TRUNCATE);
						}
						visible_enemies.push_back(e);
					}

					g_shared_esp.push_frame(local_self, visible_enemies);
				}

				// ── 2. Render entities received from server ──────────────────────────
				std::lock_guard<std::mutex> lk(g_shared_esp.ents_mtx);
				for (auto& ent : g_shared_esp.ents_from_peers) {
					vec3_t ent_pos{ ent.x, ent.y, ent.z };

					vec2_t base_pt, head_pt;
					vec3_t head_pos = ent_pos + vec3_t{ 0.f, 1.8f, 0.f };
					if (!ctx.project(ent_pos, base_pt) || !ctx.project(head_pos, head_pt))
						continue;

					const float box_h = base_pt.y - head_pt.y;
					if (box_h < 4.f) continue;
					const float box_w = box_h * 0.45f;
					vec2_t box_pos  = { head_pt.x - box_w * 0.5f, head_pt.y };
					vec2_t box_size = { box_w, box_h };

					// Peers (friends) = blue, Enemies seen by friends = red
					ImU32 box_col, name_col, dist_col, hp_col, snap_col;
					if (ent.is_peer) {
						// Friend — blue tint
						box_col  = IM_COL32(100, 160, 255, 255);
						name_col = IM_COL32(100, 200, 255, 255);
						dist_col = IM_COL32(160, 200, 255, 200);
						hp_col   = IM_COL32(100, 180, 255, 255);
						snap_col = IM_COL32(100, 160, 255, 180);
					} else {
						// Enemy seen by friend — use user-configured colors
						box_col  = ImGui::ColorConvertFloat4ToU32(config::shared_esp::box_color.value);
						name_col = ImGui::ColorConvertFloat4ToU32(config::shared_esp::name_color.value);
						dist_col = ImGui::ColorConvertFloat4ToU32(config::shared_esp::distance_color.value);
						hp_col   = ImGui::ColorConvertFloat4ToU32(config::shared_esp::health_color.value);
						snap_col = ImGui::ColorConvertFloat4ToU32(config::shared_esp::snapline_color.value);
					}

					// Box
					if (config::shared_esp::box.value)
						esp_render.rect(std::floor(box_pos.x), std::floor(box_pos.y),
							std::floor(box_size.x), std::floor(box_size.y), box_col, 1.f);

					// Name (friend tag for peers)
					if (config::shared_esp::name.value && ent.name[0]) {
						char label[48];
						if (ent.is_peer) snprintf(label, sizeof(label), "[F] %s", ent.name);
						else             snprintf(label, sizeof(label), "%s",     ent.name);
						esp_render.string(std::floor(box_pos.x + box_size.x * 0.5f),
							std::floor(box_pos.y - 14.f),
							11.f, text_outline | text_center_x, name_col, label);
					}

					// Distance
					if (config::shared_esp::distance.value) {
						float dist = sdk::local_position.distance_to(ent_pos);
						char dist_buf[16]; snprintf(dist_buf, sizeof(dist_buf), "%.0fm", dist);
						esp_render.string(std::floor(box_pos.x + box_size.x * 0.5f),
							std::floor(box_pos.y + box_size.y + 4.f),
							10.f, text_outline | text_center_x, dist_col, dist_buf);
					}

					// Health bar
					if (config::shared_esp::health.value) {
						float hp_frac = ImClamp(ent.hp / 100.f, 0.f, 1.f);
						float bar_x   = box_pos.x - 5.f;
						float bar_h   = box_size.y;
						float fill_h  = bar_h * hp_frac;
						esp_render.rect_filled(bar_x, box_pos.y, 3.f, bar_h, IM_COL32(10,10,10,200));
						esp_render.rect_filled(bar_x, box_pos.y + bar_h - fill_h, 3.f, fill_h, hp_col);
						esp_render.rect(bar_x, box_pos.y, 3.f, bar_h, IM_COL32(0,0,0,200), 1.f);
					}

					// Snapline
					if (config::shared_esp::snapline.value) {
						const ImVec2 sc{ ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y };
						esp_render.line(sc.x, sc.y,
							std::floor(box_pos.x + box_size.x * 0.5f),
							std::floor(box_pos.y + box_size.y), snap_col, 1.f);
					}
				}
			}
		}
	}
}