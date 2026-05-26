#pragma once
#include "../../sdk/sdk.h"
#include "violation.h"
#include "../../../configs/configs.h"

void viewmodel() {
	if (!config::misc::viewmodel::misc_viewmodel.value)
		return;

	auto* active_model = sdk::BaseViewModel::active_model();
	if (!memory::is_valid(active_model))
		return;

	if (config::misc::viewmodel::misc_no_viewmodel_bob.value) {
		if (auto* Bob = active_model->bob_model()) {
			Bob->set_bob_amount_run(0.f);
			Bob->set_bob_amount_walk(0.f);
			Bob->set_bob_speed_run(0.f);
			Bob->set_bob_speed_walk(0.f);
		}
	}

	if (config::misc::viewmodel::misc_no_viewmodel_sway.value) {
		if (auto* Sway = active_model->sway_model()) {
			Sway->set_positional_sway_amount(0.f);
			Sway->set_positional_sway_speed(0.f);
			Sway->set_rotation_sway_amount(0.f);
			Sway->set_rotation_sway_speed(0.f);
		}
	}

	if (config::misc::viewmodel::misc_no_viewmodel_lower.value) {
		if (auto* Lower = active_model->lower_model()) {
			Lower->set_lower_on_sprint(false);
			Lower->set_should_lower(false);
			Lower->set_lower_on_sprint(0);
		}
	}

	if (config::misc::viewmodel::misc_no_attack_animation.value) {
		if (!memory::is_valid(sdk::local_player))
			return;

		auto* animator = active_model->animator();
		if (!memory::is_valid(animator))
			return;

		auto* weapon = sdk::local_player->held_entity();
		if (!memory::is_valid(weapon))
			return;

		if (weapon->is_class(_("Hammer")) || weapon->is_class(_("Planner")))
			return;

		if (weapon->has_attack_cooldown())
			return;

		if (weapon->time_since_deploy() <= weapon->deploy_delay())
			return;

		auto* input = sdk::local_player->input();
		if (!memory::is_valid(input))
			return;

		auto* state = input->state();
		if (!memory::is_valid(state))
			return;

		if (state->is_down(enums::FIRE_PRIMARY))
			animator->set_speed(-1.f);
	}
}

void viewmodel_changer() {
	static vec3_t current_offset = { 0.f, 0.f, 0.f };

	bool changer_active = config::misc::viewmodel::misc_viewmodel_changer.value;
	if (!changer_active && current_offset.x == 0.f && current_offset.y == 0.f && current_offset.z == 0.f)
		return;

	if (!memory::is_valid(sdk::local_player))
		return;

	auto* player = sdk::local_player;
	
	// Проверяем, целится ли игрок
	bool is_aiming = false;
	auto* input = player->input();
	if (memory::is_valid(input)) {
		auto* state = input->state();
		if (memory::is_valid(state)) {
			is_aiming = state->is_down(enums::FIRE_SECONDARY);
		}
	}

	auto* held_entity = player->held_entity();
	if (!memory::is_valid(held_entity))
		return;

	auto* viewmodel = held_entity->view_model();
	if (!memory::is_valid(viewmodel)) 
		return;

	auto* view_mode_instance = viewmodel->instance();
	if (!memory::is_valid(view_mode_instance))
		return;

	vec3_t target_offset = { 0.f, 0.f, 0.f };
	if (changer_active && !is_aiming) {
		target_offset.x = config::misc::viewmodel::misc_viewmodel_X.value;
		target_offset.y = config::misc::viewmodel::misc_viewmodel_Y.value;
		target_offset.z = config::misc::viewmodel::misc_viewmodel_Z.value;
	}

	float dt = unity::Time::delta_time();
	if (dt > 0.15f) dt = 0.15f; 

	float lerp_speed = 10.f; 
	current_offset.x = current_offset.x + (target_offset.x - current_offset.x) * lerp_speed * dt;
	current_offset.y = current_offset.y + (target_offset.y - current_offset.y) * lerp_speed * dt;
	current_offset.z = current_offset.z + (target_offset.z - current_offset.z) * lerp_speed * dt;

	float dist_sq = current_offset.x * current_offset.x + current_offset.y * current_offset.y + current_offset.z * current_offset.z;

	if (!changer_active && dist_sq < 0.0001f) {
		current_offset = { 0.f, 0.f, 0.f };
		view_mode_instance->set_use_view_model_camera(true); 
		return;
	}

	if (changer_active) {
		view_mode_instance->set_use_view_model_camera(false);
	} else {
		view_mode_instance->set_use_view_model_camera(dist_sq < 0.01f);
	}

	auto* transform = view_mode_instance->transform();
	if (!memory::is_valid(transform))
		return;

	if (memory::is_valid(player->eyes())) {
		vec3_t eyesPos = player->eyes()->position();
		quat_t eyesRot = player->eyes()->rotation();

		transform->set_rotation(quat_t(eyesRot.x, eyesRot.y, eyesRot.z, eyesRot.w));

		vec3_t tForward = transform->forward();
		vec3_t tRight = transform->right();
		vec3_t tUp = transform->up();

		vec3_t finalPos = eyesPos;
		finalPos += tRight * current_offset.x;
		finalPos += tUp * current_offset.y;
		finalPos += tForward * current_offset.z;

		transform->set_position(finalPos);
	}
}

void fov_changer() {
	static float current_fov = 90.f;

	float fov_dt = unity::Time::delta_time();
	if (fov_dt > 0.15f) fov_dt = 0.15f; 

	if (core::g_unloading) {
		convar::Graphics::set_fov(90.f);
		current_fov = 90.f;
		return;
	}

	bool zooming = false;
	if (config::misc::view::misc_zoom_changer.value) {
		int key = config::misc::view::misc_zoom_key.value;
		int mode = config::misc::view::misc_zoom_key_mode.value;
		
		if (key != 0) {
			if (mode == 0) { 
				zooming = (GetAsyncKeyState(key) & 0x8000) != 0;
			}
			else if (mode == 1) { 
				static bool toggle_state = false;
				static bool was_pressed = false;
				bool is_pressed = (GetAsyncKeyState(key) & 0x8000) != 0;
				if (is_pressed && !was_pressed) {
					toggle_state = !toggle_state;
				}
				was_pressed = is_pressed;
				zooming = toggle_state;
			}
			else if (mode == 2) { 
				zooming = true;
			}
		}
	}

	float target_fov = 90.f;
	float speed = 5.f;

	if (zooming) {
		target_fov = config::misc::view::misc_zoom_value.value;
		speed = 21.f;
	}
	else if (config::misc::view::misc_fov_changer.value) {
		target_fov = config::misc::view::misc_fov_changer_amount.value;
		speed = 21.f;
	}

	current_fov = current_fov + (target_fov - current_fov) * speed * fov_dt;

	if (!zooming && !config::misc::view::misc_fov_changer.value && std::abs(current_fov - 90.f) < 0.1f) {
		current_fov = 90.f;
	}

	convar::Graphics::set_fov(current_fov);
}

void aspect_ratio_changer() {
	float default_aspect = 1.777f;
	float w = (float)unity::Screen::width();
	float h = (float)unity::Screen::height();
	if (h > 0.f) {
		default_aspect = w / h;
	}

	static float current_ratio = default_aspect;
	static bool first_run = true;
	if (first_run) {
		current_ratio = default_aspect;
		first_run = false;
	}

	float aspect_dt = unity::Time::delta_time();
	if (aspect_dt > 0.15f) aspect_dt = 0.15f; 

	if (core::g_unloading) {
		if (memory::is_valid(sdk::camera)) {
			sdk::camera->set_aspect(default_aspect);
		}
		current_ratio = default_aspect;
		return;
	}

	if (config::misc::view::misc_aspect_ratio_changer.value) {
		float target_ratio = config::misc::view::misc_aspect_ratio_value.value;
		if (target_ratio < 0.5f) target_ratio = 0.5f;
		if (target_ratio > 3.0f) target_ratio = 3.0f;

		current_ratio = current_ratio + (target_ratio - current_ratio) * 10.f * aspect_dt;

		if (current_ratio >= 0.5f && current_ratio <= 3.0f) {
			if (memory::is_valid(sdk::camera)) {
				sdk::camera->set_aspect(current_ratio);
			}
		}
	}
	else {
		current_ratio = current_ratio + (default_aspect - current_ratio) * 5.f * aspect_dt;
		if (std::abs(current_ratio - default_aspect) < 0.01f) {
			current_ratio = default_aspect;
		}

		if (memory::is_valid(sdk::camera)) {
			sdk::camera->set_aspect(current_ratio);
		}
	}
}

inline int original_view_mode = 0;
inline float original_cam_dist = 2.f;
inline bool camera_mode_was_active = false;
constexpr float kDefaultCamDist = 2.f;

inline vec3_t bullet_camera_pos = {0.f, 0.f, 0.f};
inline quat_t bullet_camera_rot = {0.f, 0.f, 0.f, 1.f};
inline bool bullet_camera_active = false;

inline void reset_camera_mode(sdk::BasePlayer* player) {
	if (!memory::is_valid(player)) return;

	if (camera_mode_was_active) {
		player->set_selected_view_mode(original_view_mode);
		convar::Client::set_cam_dist(original_cam_dist);
		camera_mode_was_active = false;
	}
}

inline void update_camera_mode(sdk::BasePlayer* player) {
	if (!memory::is_valid(player) || core::g_unloading) return;

	bool enabled = config::misc::view::misc_camera_mode.value;
	int mode = config::misc::view::misc_camera_mode_value.value;
	if (mode < 0) mode = 0;
	if (mode > 3) mode = 3;

	float dist = config::misc::view::misc_camera_mode_dist.value;
	if (dist < 0.f) dist = 0.f;
	if (dist > 10.f) dist = 10.f;

	int key = config::misc::view::misc_camera_mode_key.value;
	int key_mode = config::misc::view::misc_camera_mode_key_mode.value;

	bool is_key_down = false;
	if (key != 0) {
		if (key_mode == 0) { // Hold
			is_key_down = (GetAsyncKeyState(key) & 0x8000) != 0;
		}
		else if (key_mode == 1) { // Toggle
			static bool toggle_state = false;
			static bool was_pressed = false;
			bool is_pressed = (GetAsyncKeyState(key) & 0x8000) != 0;
			if (is_pressed && !was_pressed) {
				toggle_state = !toggle_state;
			}
			was_pressed = is_pressed;
			is_key_down = toggle_state;
		}
		else if (key_mode == 2) { // Always on
			is_key_down = true;
		}
	}

	bool should_be_active = enabled && is_key_down;

	if (should_be_active) {
		if (!camera_mode_was_active) {
			original_view_mode = player->selected_view_mode();
			original_cam_dist = convar::Client::cam_dist();
			camera_mode_was_active = true;
		}

		if (player->selected_view_mode() != mode)
			player->set_selected_view_mode(mode);

		float target_dist = mode == 0 ? kDefaultCamDist : dist;
		float current_dist = convar::Client::cam_dist();
		if (std::abs(current_dist - target_dist) > 0.01f)
			convar::Client::set_cam_dist(target_dist);
	}
	else {
		reset_camera_mode(player);
	}
}

inline void camera_mode() {
	if (!memory::is_valid(sdk::local_player))
		return;

	// 1. Normal camera mode (FPP/TPP distance overrides)
	update_camera_mode(sdk::local_player);
}

inline void auto_reload() {
	if (!memory::is_valid(sdk::local_player) || core::g_unloading)
		return;

	if (!config::misc::automatic::misc_automatic_auto_reload.value)
		return;

	auto* held_entity = sdk::local_player->held_entity();
	if (!memory::is_valid(held_entity))
		return;

	if (!held_entity->is_class(_("BaseProjectile")))
		return;

	static float cached_shot_time = 0.f;
	static bool shot_detected = false;
	static bool reload_started = false;

	float memory_shot_time = held_entity->last_shot_time();
	float time = unity::Time::time();

	if (held_entity->next_reload_time() > time)
		return;

	if (memory_shot_time != cached_shot_time) {
		cached_shot_time = memory_shot_time;
		shot_detected = true;
		reload_started = false;
	}

	if (!held_entity->has_reload_cooldown() && shot_detected) {
		auto* mag = held_entity->primary_magazine();
		if (!memory::is_valid(mag))
			return;

		int ammo = mag->contents();
		int capacity = mag->capacity();

		if (ammo >= capacity) {
			shot_detected = false;
			return;
		}

		float time_since = time - memory_shot_time;

		if (time_since > 2.0f && !reload_started) {
			held_entity->server_rpc(_("StartReload"));
			held_entity->set_next_reload_time(time + held_entity->reload_time());
			held_entity->send_signal_broadcast(enums::e_signal::reload, _(""));
			reload_started = true;
		}

		float finish_time = held_entity->reload_time() * 0.9f;
		if (time_since > (2.0f + finish_time) && reload_started) {
			held_entity->server_rpc(_("Reload"));
			shot_detected = false;
			reload_started = false;
		}
	}
}

inline void fake_admin() {
	if (!memory::is_valid(sdk::local_player))
		return;

	if (config::misc::exploits::exploits_admin_flag.value && !core::g_unloading) {
		sdk::local_player->add_flag(enums::e_flag::is_admin);
	}
	else {
		sdk::local_player->remove_flag(enums::e_flag::is_admin);
	}
}

inline void movement_action() {
	if (!memory::is_valid(sdk::local_player) || core::g_unloading)
		return;

	auto* movement = sdk::local_player->walk_movement();
	if (!memory::is_valid(movement))
		return;

	bool walk_trees = config::misc::movement::exploits_walk_through_trees.value;
	bool walk_players = config::misc::movement::exploits_walk_through_players.value;
	bool walk_water = config::misc::movement::exploits_walk_on_water.value;

	unity::Physics::ignore_layer_collision(static_cast<enums::e_layer_collision>(30), enums::e_layer_collision::player_movement, walk_trees);
	unity::Physics::ignore_layer_collision(enums::e_layer_collision::ai, enums::e_layer_collision::player_movement, walk_players);
	unity::Physics::ignore_layer_collision(enums::e_layer_collision::water, enums::e_layer_collision::player_movement, !walk_water);

	if (!movement->flying()) {
		if (config::misc::movement::exploits_omni_sprint.value) {
			vec3_t vel = movement->target_movement();
			float vel_length = vel.length();

			if (vel_length > 0.01f) {
				auto* model_state = sdk::local_player->model_state();
				if (memory::is_valid(model_state))
					model_state->set_sprinting(true);

				bool is_slowed = sdk::local_player->is_swimming() || (memory::is_valid(model_state) && model_state->ducked());
				float max_speed = is_slowed ? 1.7f : 5.5f;
				float scale = max_speed / vel_length;

				movement->set_target_movement(vec3_t(vel.x * scale, vel.y, vel.z * scale));
			}
		}

		if (config::misc::movement::misc_infinite_jump.value) {
			movement->set_ground_time(movement->ground_time() + 0.5f);
			movement->set_land_time(0.f);
			movement->set_jump_time(0.f);
		}

		bool suicide_active = config::misc::movement::misc_suicide.value && config::misc::movement::misc_suicide_key.value != 0 && (GetAsyncKeyState(config::misc::movement::misc_suicide_key.value) & 0x8000) != 0;

		if (suicide_active) {
			movement->set_was_falling(true);
			movement->set_previous_velocity(vec3_t(0.f, -1000.f, 0.f));
			movement->set_ground_time(0.f);
		}
	}

	if (config::misc::movement::misc_climb_assist.value) {
		movement->set_ground_angle_new(0.f);
		movement->set_ground_angle(0.f);

		if (config::misc::movement::misc_climb_assist_bypass.value) {
			auto* capsule = movement->capsule();
			if (memory::is_valid(capsule))
				capsule->set_radius(0.44f);
		}
	}

	if (config::misc::movement::misc_always_sprint.value) {
		convar::Input::set_sprinting(true);
	}
}

inline void time_changer() {
	static float current_smooth_time = -1.0f;

	bool enabled = config::visuals::worlds::visuals_time_change_time.value && !core::g_unloading;
	float target_time = config::visuals::worlds::visuals_time_change_time_value.value;

	if (!enabled) {
		if (current_smooth_time != -1.0f) {
			convar::Admin::set_admintime(-1.0f);
			current_smooth_time = -1.0f;
		}
		return;
	}

	if (current_smooth_time < 0.0f) {
		current_smooth_time = target_time;
	}

	const float speed = 5.0f;
	float dt = unity::Time::delta_time();

	if (std::abs(current_smooth_time - target_time) > 0.01f) {
		current_smooth_time = std::lerp(current_smooth_time, target_time, speed * dt);
	}
	else {
		current_smooth_time = target_time;
	}

	convar::Admin::set_admintime(current_smooth_time);
}

inline void anti_fly_hack_kick() {
	violation::tick();
}

inline void fast_heal() {
	if (!memory::is_valid(sdk::local_player) || core::g_unloading)
		return;

	if (!config::misc::automatic::misc_automatic_auto_heal.value)
		return;

	auto* active_item = sdk::local_player->item();
	if (!memory::is_valid(active_item))
		return;

	if (!active_item->is_heal())
		return;

	auto* held_entity = active_item->held_entity();
	if (!memory::is_valid(held_entity))
		return;

	static float last_heal_time = 0.f;
	float current_time = sdk::local_player->last_sent_tick_time();

	if ((current_time - last_heal_time) <= 0.7f)
		return;

	float health = sdk::local_player->health();
	if (health >= 99.f)
		return;

	auto* player_model = sdk::local_player->model();
	if (!memory::is_valid(player_model))
		return;

	vec3_t local_pos = player_model->position();

	float closest_distance = 999999.f;
	sdk::BasePlayer* closest_player = nullptr;

	auto* player_list = sdk::BasePlayer::visible_player_list();
	if (memory::is_valid(player_list)) {
		int player_count = player_list->count();
		for (int i = 0; i < player_count; ++i) {
			auto* player = player_list->get<sdk::BasePlayer*>(i);
			if (!memory::is_valid(player) || player == sdk::local_player)
				continue;

			if (player->is_dead())
				continue;

			auto* target_model = player->model();
			if (!memory::is_valid(target_model))
				continue;

			vec3_t target_pos = target_model->position();
			float dist = local_pos.distance_to(target_pos);

			if (dist < closest_distance) {
				closest_distance = dist;
				closest_player = player;
			}
		}
	}

	float tsd = held_entity->time_since_deploy();
	float dd = held_entity->deploy_delay();
	float nat = held_entity->next_attack_time();
	float time = unity::Time::time();

	if (tsd > dd && nat <= time) {
		if (closest_distance < 5.f && memory::is_valid(closest_player)) {
			held_entity->server_rpc(_("UseOther"));
			last_heal_time = current_time;
		}
		else {
			held_entity->server_rpc(_("UseSelf"));
			last_heal_time = current_time;
		}
	}
}

void attack_actions() {
	if (!memory::is_valid(sdk::local_player))
		return;

	if (config::misc::exploits::misc_modify_can_attack.value) {
		auto* movement = sdk::local_player->walk_movement();
		if (memory::is_valid(movement)) {
			auto* model_state = sdk::local_player->model_state();
			if (memory::is_valid(model_state))
				model_state->add_flag(enums::e_model_state_flags::on_ground);

			movement->set_grounded_backing(1.f);
		}
	}

	if (config::misc::exploits::exploits_unlock_aim_on_jugger_set.value) {
		sdk::local_player->set_clothing_blocks_aiming(false);
	}

	if (config::misc::exploits::exploits_can_attack_in_vehicles.value) {
		auto* mounted = sdk::local_player->mounted();
		if (memory::is_valid(mounted)) {
			mounted->set_can_wield_items(true);
		}
	}
}

inline bool g_traps_active = false;

void exploits_actions() {
	if (!memory::is_valid(sdk::local_player) || core::g_unloading)
		return;

	static bool fl_in_window = false;
	static float fl_last_toggle = 0.f;
	static float original_tick_interval = 0.05f;

	bool traps_active = false;
	if (config::misc::exploits::exploits_traps_exploit.value) {
		int key = config::misc::exploits::exploits_traps_exploit_key.value;
		int mode = config::misc::exploits::exploits_traps_exploit_keymode.value;
		if (key != 0) {
			if (mode == 0) {
				traps_active = (GetAsyncKeyState(key) & 0x8000) != 0;
			} else if (mode == 1) {
				static bool toggle = false;
				static bool pressed = false;
				bool is_pressed = (GetAsyncKeyState(key) & 0x8000) != 0;
				if (is_pressed && !pressed) toggle = !toggle;
				pressed = is_pressed;
				traps_active = toggle;
			} else {
				traps_active = true;
			}
		}
	}

	g_traps_active = traps_active;

	if (traps_active) {
		sdk::local_player->set_client_tick_interval(config::misc::exploits::exploits_traps_exploit_amount.value);
		fl_in_window = false;
	}
	else if (config::misc::exploits::exploits_fake_lag.value) {
		float current_time = unity::Time::time();

		if (!fl_in_window) {
			if (current_time - fl_last_toggle >= config::misc::exploits::exploits_fake_lag_cooldown.value) {
				fl_in_window = true;
				fl_last_toggle = current_time;
			}
			else {
				sdk::local_player->set_client_tick_interval(original_tick_interval);
			}
		}
		else {
			float elapsed = current_time - fl_last_toggle;
			if (elapsed > config::misc::exploits::exploits_fake_lag_duration.value) {
				fl_in_window = false;
				fl_last_toggle = current_time;
				sdk::local_player->set_client_tick_interval(original_tick_interval);
			}
			else {
				float min_val = config::misc::exploits::exploits_fake_lag_min.value;
				float max_val = config::misc::exploits::exploits_fake_lag_max.value;
				float random_tick = min_val + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max_val - min_val)));
				sdk::local_player->set_client_tick_interval(random_tick);
			}
		}
	}
	else {
		sdk::local_player->set_client_tick_interval(original_tick_interval);
		fl_in_window = false;
	}
}