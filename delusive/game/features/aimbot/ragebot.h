#pragma once
#include "../../sdk/sdk.h"
#include "../../../configs/configs.h"
#include "../../sdk/game_physics.h"
#include "../prediction/prediction.h"
#include "../../sdk/protobuf/projectileshoot.h"
#include <cmath>
#include <algorithm>
#include <vector>
#include <algorithm>

namespace aimbot {
	namespace ragebot {

		struct rage_target_t {
			sdk::BasePlayer* player = nullptr;
			vec3_t position{};
			vec3_t velocity{};
			int bone_id = -1;
		};

		static inline sdk::BasePlayer* g_silent_target = nullptr;
		static inline vec3_t g_last_predicted_position{};

		inline float fov_to_px(float fov, float center_y) {
			return (fov * center_y) / 90.f;
		}

		inline bool is_target_valid(sdk::BasePlayer* player) {
			if (!memory::is_valid(player) || player->lifestate() || player->is_local_player())
				return false;

			const auto& filters = config::aimbot::filters.value;
			if (std::find(filters.begin(), filters.end(), 0) != filters.end() && player->is_teammate()) return false;
			if (std::find(filters.begin(), filters.end(), 1) != filters.end() && player->is_sleeping()) return false;

			auto* model = player->model();
			bool  is_npc = memory::is_valid(model) && model->is_npc();
			if (std::find(filters.begin(), filters.end(), 2) != filters.end() && is_npc) return false;
			if (std::find(filters.begin(), filters.end(), 3) != filters.end() && player->is_knocked()) return false;

			return true;
		}

		inline rage_target_t find_best_target(const vec3_t& local_eyes, float fov_limit) {
			rage_target_t best{};
			float best_fov = FLT_MAX;

			const vec2_t center = {
				static_cast<float>(unity::Screen::width()) * 0.5f,
				static_cast<float>(unity::Screen::height()) * 0.5f
			};

			const float fov_px = fov_to_px(fov_limit, center.y);
			std::vector<sdk::BasePlayer*> players;
			{
				std::lock_guard<std::mutex> lock(sdk::info.mtx);
				players = sdk::info.players;
			}
			
			const auto& scan_bones = config::aimbot::scan_bones.value;
			if (scan_bones.empty()) return best;

			for (auto* player : players) {
				if (!is_target_valid(player)) continue;

				const vec3_t vel = player->get_world_velocity();

				for (int b : scan_bones) {
					enums::e_bone e_bone = enums::e_bone::head;
					if (b == 0) e_bone = enums::e_bone::head;
					else if (b == 1) e_bone = enums::e_bone::neck;
					else if (b == 2) e_bone = enums::e_bone::spine4;
					else if (b == 3) e_bone = enums::e_bone::spine3;
					else if (b == 4) e_bone = enums::e_bone::spine1;
					else if (b == 5) e_bone = enums::e_bone::pelvis;
					else if (b == 6) e_bone = enums::e_bone::l_hip;
					else if (b == 7) e_bone = enums::e_bone::r_hip;

					const vec3_t bone_pos = player->bone_position(e_bone);
					if (bone_pos.empty()) continue;

					const float dist = local_eyes.distance_to(bone_pos);
					if (dist > config::aimbot::max_distance.value) continue;

					vec2_t screen_pos;
					auto ctx = unity::Camera::get_frame_context();
					if (!ctx.project(bone_pos, screen_pos)) continue;

					float fov_metric = screen_pos.distance_to(center);
					if (fov_metric > fov_px || fov_metric >= best_fov) continue;

					if (config::aimbot::visible_check.value)
						if (!sdk::GamePhysics::is_visible(local_eyes, bone_pos)) continue;

					best.player = player;
					best.position = bone_pos;
					best.velocity = vel;
					best.bone_id = (int)e_bone;
					best_fov = fov_metric;
				}
			}

			return best;
		}

		inline bool is_aim_key_active() {
			const int key  = config::aimbot::rage_aim_key.value;
			const int mode = config::aimbot::rage_aim_key_mode.value;
			if (mode == 2) return true; // Always On
			if (key == 0)  return false;
			if (mode == 0) return (GetAsyncKeyState(key) & 0x8000) != 0; // Hold
			if (mode == 1) return (GetKeyState(key) & 0x0001) != 0;      // Toggle
			return false;
		}

		inline void run_silent_tracking() {
			auto* local = sdk::local_player;
			if (!memory::is_valid(local) || local->lifestate() != 0 || local->is_sleeping()) {
				g_silent_target = nullptr;
				return;
			}

			if (!is_aim_key_active()) {
				g_silent_target = nullptr;
				return;
			}

			auto* eyes = local->eyes();
			if (!memory::is_valid(eyes)) return;
			vec3_t local_eyes = eyes->position();

			bool is_aiming = false;
			if (auto* ms = local->model_state(); memory::is_valid(ms))
				is_aiming = ms->has_flag(enums::e_model_state_flags::in_aim);

			float base_fov  = config::aimbot::fov.value * 1.1f;
			float fov_limit = (config::aimbot::dynamic_fov.value && is_aiming) ? base_fov * 1.4f : base_fov;

			rage_target_t target = find_best_target(local_eyes, fov_limit);

			if (!target.player) {
				g_silent_target = nullptr;
				return;
			}

			g_silent_target = target.player;

			auto pred = prediction::c_prediction::run(local_eyes, target.position, target.velocity, 0.2f, -1.f, 8.f, -1.f, -1.f);
			if (pred.possible) {
				g_last_predicted_position = pred.position;
			}
		}

		inline void process_shoot_rpc(void* projectile_shoot_rpc) {
			auto* rpc = reinterpret_cast<protobuf::ProjectileShoot*>(projectile_shoot_rpc);
			if (!memory::is_valid(rpc)) return;

			auto* local = sdk::local_player;
			if (!memory::is_valid(local)) return;

			vec3_t final_velocity{};
			float final_initial_dist = 0.f;
			bool computed = false;

			if (config::aimbot::rage_enabled.value) {
				if (is_aim_key_active()) {
					auto* eyes = local->eyes();
					if (memory::is_valid(eyes)) {
						vec3_t local_eyes = eyes->position();

						bool is_aiming = false;
						if (auto* ms = local->model_state(); memory::is_valid(ms))
							is_aiming = ms->has_flag(enums::e_model_state_flags::in_aim);

						float base_fov  = config::aimbot::fov.value * 1.1f;
						float fov_limit = (config::aimbot::dynamic_fov.value && is_aiming) ? base_fov * 1.4f : base_fov;

						rage_target_t target = find_best_target(local_eyes, fov_limit);

						if (target.player) {
							g_silent_target = target.player;
							auto* rpc_projectiles = rpc->projectiles();

							if (memory::is_valid(rpc_projectiles)) {
								for (std::uint32_t i = 0; i < rpc_projectiles->count(); ++i) {
									auto* rpc_proj = rpc_projectiles->value(i);
									if (!memory::is_valid(rpc_proj)) continue;

									vec3_t start_pos = rpc_proj->start_pos();
									vec3_t orig_vel = rpc_proj->start_vel();
									if (start_pos.empty() || orig_vel.empty()) continue;

									if (!computed) {
										float speed = orig_vel.length();
										float initial_dist = 0.f;

										if (config::weapons::instant_bullet.value || config::aimbot::rage_bullet_tp.value) {
											float realtime = unity::Time::realtime_since_startup();
											float last_tick = local->last_sent_tick_time();
											float desync = std::clamp(realtime - last_tick, 0.f, 1.f);
											float dsv = max(desync - (0.03125f * 5.f), 0.2f);

											initial_dist = dsv * speed;
											final_initial_dist = initial_dist;
										}

										vec3_t aim_target_pos = target.position;

										vec3_t aim_point = prediction::c_prediction::get_aim_direction(
											start_pos, aim_target_pos, target.velocity, speed, initial_dist);

										if (!aim_point.empty()) {
											g_last_predicted_position = aim_point;
											final_velocity = (aim_point - start_pos).normalized() * speed;
											computed = true;
										}
									}

									if (computed) {
										int rnd = (rand() % 100) + 1;
										if (rnd <= (int)config::aimbot::rage_hitchance.value)
											rpc_proj->set_start_vel(final_velocity);
									}
								}
							}
						}
					}
				}
			}

			auto* held_item = local->item();
			if (!memory::is_valid(held_item)) return;
			auto* held_entity = held_item->held_entity();
			if (!memory::is_valid(held_entity)) return;

			auto* created_projectiles = reinterpret_cast<unity::c_list<sdk::Projectile*>*>(held_entity->created_projectiles());
			if (!memory::is_valid(created_projectiles)) return;

			uint32_t proj_size = created_projectiles->count();
			if (proj_size == 0 || proj_size > 128) return;

			for (std::uint32_t i = 0; i < proj_size; ++i) {
				auto* local_proj = created_projectiles->value(i);
				if (!memory::is_valid(local_proj)) continue;

				if (computed) {
					local_proj->set_initial_velocity(final_velocity);
					local_proj->set_current_velocity(final_velocity);
				}

				if (config::weapons::thick_bullet.value) {
					float thick_val = config::weapons::thick_bullet_size.value;
					if (thick_val <= 0.1f) thick_val = 0.75f;
					local_proj->set_thickness(thick_val);
				} else {
					local_proj->set_thickness(0.1f);
				}

				if (config::weapons::instant_bullet.value || config::aimbot::rage_bullet_tp.value) {
					if (!computed) {
						float speed = local_proj->initial_velocity().length();
						float realtime = unity::Time::realtime_since_startup();
						float last_tick = local->last_sent_tick_time();
						float desync = std::clamp(realtime - last_tick, 0.f, 1.f);
						float dsv = max(desync - (0.03125f * 5.f), 0.2f);
						final_initial_dist = dsv * speed;
					}
					local_proj->set_initial_distance(final_initial_dist);
				}
			}
		}

		inline void run() {
			if (!sdk::local_player || !memory::is_valid(sdk::local_player))
				return;
			if (config::aimbot::rage_enabled.value) {
				run_silent_tracking();
			} else {
				g_silent_target = nullptr;
			}
		}
	}
}