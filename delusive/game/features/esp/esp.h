#pragma once
#include <algorithm>
#include <cmath>
#include "../../sdk/sdk.h"
#include "../../../configs/configs.h"
#include "../../../core/esp_renderer.h"
#include "../../../vcruntime/logger/logger.h"

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
			for (const auto& bone_pair : bones) {
				vec2_t point1, point2;
				if (ctx.project(player->bone_position(bone_pair.first), point1) && 
				    ctx.project(player->bone_position(bone_pair.second), point2)) {
					esp_render.line(point1.x, point1.y, point2.x, point2.y, skeleton_color, thickness);
				}
			}
		}

		if (config::esp::players_name) {
			const auto& player_name = player->display_name();
			if (player_name) {
				std::string name_str = player_name->string();
				ImU32 name_color = (ImColor)config::esp::players_name_color;
				esp_render.string(std::floor(box_position.x + box_size.x / 2.f), std::floor(box_position.y - 18.f), 14.f, text_center_x | text_outline, name_color, name_str.c_str());
			}
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
			
			esp_render.rect_filled(std::floor(box_position.x - 6.f), std::floor(box_position.y - 1.f), 4.f, std::floor(box_size.y + 2.f), bg_color);
			esp_render.rect_filled(std::floor(box_position.x - 5.f), bar_y, 2.f, bar_h, hp_color);
		}

		if (config::esp::players_weapon) {
			const auto& item = player->item();
			if (item) {
				const auto& info = item->info();
				if (memory::is_valid(info)) {
					const auto& shortname = info->shortname();
					if (memory::is_valid(shortname)) {
						std::string weapon_name = shortname->string();
						ImU32 weapon_color = (ImColor)config::esp::players_weapon_color;
						esp_render.string(std::floor(box_position.x + box_size.x / 2.f), std::floor(box_position.y + box_size.y + 2.f), 14.f, text_center_x | text_outline, weapon_color, weapon_name.c_str());
					}
				}
			}
		}
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

		if (!sdk::MapInterface::is_open()) {
			auto ctx = unity::Camera::get_frame_context();
			for (auto&& player : sdk::info.players) {
				render_player(player, ctx);
			}
		}
	}
}