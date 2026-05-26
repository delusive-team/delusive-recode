#pragma once
#include "../il2cpp/il2cpp.h"
#include "base_combat_entity.h"
#include "player_eyes.h"
#include "player_input.h"
#include "base_mountable.h"
#include "player_model.h"
#include "model_state.h"
#include "player_inventory.h"
#include "player_walk_movement.h"
#include "player_tick.h"
#include "world_item.h"
#include "math/vector.h"
#include "math/bounds.h"
#include "unity/capsule_collider.h"
#include "unity/string.h"
#include "protobuf/player_projectile_update.h"
#include "enums/enums.h"
#include "held_entity.h"
#include "item.h"
#include "player_team.h"
#include "local_player.h"
#include "item_container.h"
#include <vector>
#include <map>
#include <unordered_map>
#include "main_camera.h"
#include "bundles.h"

namespace sdk {
	inline std::vector<enums::e_bone> bones = {
		enums::e_bone::pelvis,
		enums::e_bone::l_hip,
		enums::e_bone::l_knee,
		enums::e_bone::l_foot,
		enums::e_bone::l_toe,
		enums::e_bone::l_ankle_scale,
		enums::e_bone::penis,
		enums::e_bone::GenitalCensor,
		enums::e_bone::GenitalCensor_LOD0,
		enums::e_bone::Inner_LOD0,
		enums::e_bone::GenitalCensor_LOD1,
		enums::e_bone::GenitalCensor_LOD2,
		enums::e_bone::r_hip,
		enums::e_bone::r_knee,
		enums::e_bone::r_foot,
		enums::e_bone::r_toe,
		enums::e_bone::r_ankle_scale,
		enums::e_bone::spine1,
		enums::e_bone::spine1_scale,
		enums::e_bone::spine2,
		enums::e_bone::spine3,
		enums::e_bone::spine4,
		enums::e_bone::l_clavicle,
		enums::e_bone::l_upperarm,
		enums::e_bone::l_forearm,
		enums::e_bone::l_hand,
		enums::e_bone::l_index1,
		enums::e_bone::l_index2,
		enums::e_bone::l_index3,
		enums::e_bone::l_little1,
		enums::e_bone::l_little2,
		enums::e_bone::l_little3,
		enums::e_bone::l_middle1,
		enums::e_bone::l_middle2,
		enums::e_bone::l_middle3,
		enums::e_bone::l_prop,
		enums::e_bone::l_ring1,
		enums::e_bone::l_ring2,
		enums::e_bone::l_ring3,
		enums::e_bone::l_thumb1,
		enums::e_bone::l_thumb2,
		enums::e_bone::l_thumb3,
		enums::e_bone::IKtarget_righthand_min,
		enums::e_bone::IKtarget_righthand_max,
		enums::e_bone::l_ulna,
		enums::e_bone::neck,
		enums::e_bone::head,
		enums::e_bone::jaw,
		enums::e_bone::eyeTranform,
		enums::e_bone::l_eye,
		enums::e_bone::l_Eyelid,
		enums::e_bone::r_eye,
		enums::e_bone::r_Eyelid,
		enums::e_bone::r_clavicle,
		enums::e_bone::r_upperarm,
		enums::e_bone::r_forearm,
		enums::e_bone::r_hand,
		enums::e_bone::r_index1,
		enums::e_bone::r_index2,
		enums::e_bone::r_index3,
		enums::e_bone::r_little1,
		enums::e_bone::r_little2,
		enums::e_bone::r_little3,
		enums::e_bone::r_middle2,
		enums::e_bone::r_middle3,
		enums::e_bone::r_prop,
		enums::e_bone::r_ring1,
		enums::e_bone::r_ring2,
		enums::e_bone::r_ring3,
		enums::e_bone::r_thumb1,
		enums::e_bone::r_thumb2,
		enums::e_bone::r_thumb3,
		enums::e_bone::IKtarget_lefthand_min,
		enums::e_bone::IKtarget_lefthand_max,
		enums::e_bone::r_ulna,
		enums::e_bone::l_breast,
		enums::e_bone::r_breast,
		enums::e_bone::BoobCensor,
		enums::e_bone::BreastCensor_LOD0,
		enums::e_bone::BreastCensor_LOD1,
		enums::e_bone::BreastCensor_LOD2
	};

	class BasePlayer;

	struct PlayerCache {
		std::vector< BasePlayer* > players = { };

		std::unordered_map< BasePlayer*, bool > visible_states = { };
		std::unordered_map< BasePlayer*, enums::e_player_state > player_states = { };
		std::unordered_map< BasePlayer*, std::unordered_map< enums::e_bone, vec3_t > > bone_positions = { };
	};

	inline PlayerCache info;

	class BasePlayer : public BaseCombatEntity {
	private:
		static inline uintptr_t eyes_ptr = 0;
		static inline uintptr_t input_ptr = 0;
		static inline uintptr_t mounted_ptr = 0;
		static inline uintptr_t last_sent_tick_time_ptr = 0;
		static inline uintptr_t flags_ptr = 0;
		static inline uintptr_t last_headshot_sound_time_ptr = 0;
		static inline uintptr_t active_item_ptr = 0;
		static inline uintptr_t user_id_ptr = 0;
		static inline uintptr_t model_ptr = 0;
		static inline uintptr_t model_state_ptr = 0;
		static inline uintptr_t inventory_ptr = 0;
		static inline uintptr_t walk_movement_ptr = 0;
		static inline uintptr_t blueprints_ptr = 0;
		static inline uintptr_t client_team_ptr = 0;
		static inline uintptr_t display_name_ptr = 0;
		static inline uintptr_t collider_ptr = 0;
		static inline uintptr_t metabolism_ptr = 0;
		static inline uintptr_t selected_view_mode_ptr = 0;
		static inline uintptr_t clothing_move_speed_reduction_ptr = 0;
		static inline uintptr_t frozen_ptr = 0;
		static inline uintptr_t client_tick_interval_ptr = 0;
		static inline uintptr_t clothing_blocks_aiming_ptr = 0;
		static inline uintptr_t visible_player_list_ptr = 0;

		static inline uintptr_t on_land_ptr = 0;
		static inline uintptr_t get_speed_ptr = 0;
		static inline uintptr_t on_ladder_ptr = 0;
		static inline uintptr_t is_ducked_ptr = 0;
		static inline uintptr_t get_bounds_ptr = 0;
		static inline uintptr_t get_radius_ptr = 0;
		static inline uintptr_t get_height_ptr = 0;
		static inline uintptr_t is_running_ptr = 0;
		static inline uintptr_t is_crawling_ptr = 0;
		static inline uintptr_t is_swimming_ptr = 0;
		static inline uintptr_t max_velocity_ptr = 0;
		static inline uintptr_t get_max_speed_ptr = 0;
		static inline uintptr_t bounds_padding_ptr = 0;
		static inline uintptr_t get_jump_height_ptr = 0;
		static inline uintptr_t get_held_entity_ptr = 0;
		static inline uintptr_t send_client_tick_ptr = 0;
		static inline uintptr_t world_space_bounds_ptr = 0;
		static inline uintptr_t get_mount_velocity_ptr = 0;
		static inline uintptr_t on_view_mode_changed_ptr = 0;
		static inline uintptr_t send_projectile_update_ptr = 0;
		static inline uintptr_t pivot_point_ptr = 0;
		static inline uintptr_t closest_point_ptr = 0;
		static inline uintptr_t last_sent_tick_ptr = 0;

	public:
		static void initialize() {
			auto klass = il2cpp::get_class(_(""), _("BasePlayer"));

			il2cpp_field_offset(eyes_ptr, _(""), _("BasePlayer"), _("eyes"));
			il2cpp_field_offset(input_ptr, _(""), _("BasePlayer"), _("input"));
			il2cpp_field_offset(mounted_ptr, _(""), _("BasePlayer"), _("mounted"));
			il2cpp_field_offset(last_sent_tick_time_ptr, _(""), _("BasePlayer"), _("lastSentTickTime"));
			il2cpp_field_offset(flags_ptr, _(""), _("BasePlayer"), _("playerFlags"));
			il2cpp_field_offset(last_headshot_sound_time_ptr, _(""), _("BasePlayer"), _("lastHeadshotSoundTime"));
			il2cpp_field_offset(active_item_ptr, _(""), _("BasePlayer"), _("clActiveItem"));
			il2cpp_field_offset(user_id_ptr, _(""), _("BasePlayer"), _("userID"));
			il2cpp_field_offset(model_ptr, _(""), _("BasePlayer"), _("playerModel"));
			il2cpp_field_offset(model_state_ptr, _(""), _("BasePlayer"), _("modelState"));
			il2cpp_field_offset(inventory_ptr, _(""), _("BasePlayer"), _("inventory"));
			il2cpp_field_offset(walk_movement_ptr, _(""), _("BasePlayer"), _("movement"));
			il2cpp_field_offset(blueprints_ptr, _(""), _("BasePlayer"), _("blueprints"));
			il2cpp_field_offset(client_team_ptr, _(""), _("BasePlayer"), _("clientTeam"));
			il2cpp_field_offset(display_name_ptr, _(""), _("BasePlayer"), _("_displayName"));
			il2cpp_field_offset(collider_ptr, _(""), _("BasePlayer"), _("playerCollider"));
			il2cpp_field_offset(metabolism_ptr, _(""), _("BasePlayer"), _("metabolism"));
			il2cpp_field_offset(selected_view_mode_ptr, _(""), _("BasePlayer"), _("selectedViewMode"));
			il2cpp_field_offset(clothing_move_speed_reduction_ptr, _(""), _("BasePlayer"), _("clothingMoveSpeedReduction"));
			il2cpp_field_offset(frozen_ptr, _(""), _("BasePlayer"), _("Frozen"));
			il2cpp_field_offset(client_tick_interval_ptr, _(""), _("BasePlayer"), _("clientTickInterval"));
			il2cpp_field_offset(clothing_blocks_aiming_ptr, _(""), _("BasePlayer"), _("clothingBlocksAiming"));
			il2cpp_static_field(visible_player_list_ptr, _(""), _("BasePlayer"), _("visiblePlayerList"));
			il2cpp_static_field(last_sent_tick_ptr, _(""), _("BasePlayer"), _("lastSentTick"));

			on_land_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("OnLand"), -1));
			get_speed_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("GetSpeed"), 3));
			on_ladder_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("OnLadder"), -1));
			is_ducked_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("IsDucked"), -1));
			get_bounds_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("GetBounds"), -1));
			get_radius_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("GetRadius"), -1));
			get_height_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("GetHeight"), 1));
			is_running_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("IsRunning"), -1));
			is_crawling_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("IsCrawling"), -1));
			is_swimming_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("IsSwimming"), -1));
			max_velocity_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("MaxVelocity"), -1));
			get_max_speed_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("GetMaxSpeed"), -1));
			bounds_padding_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("BoundsPadding"), 0));
			get_jump_height_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("GetJumpHeight"), -1));
			get_held_entity_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("GetHeldEntity"), -1));
			send_client_tick_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("SendClientTick"), -1));
			world_space_bounds_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("WorldSpaceBounds"), -1));
			get_mount_velocity_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("GetMountVelocity"), 0));
			on_view_mode_changed_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("OnViewModeChanged"), -1));
			send_projectile_update_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("SendProjectileUpdate"), 1));
			
			pivot_point_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_(""), _("BaseEntity")), _("PivotPoint"), -1));
			closest_point_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_(""), _("BaseEntity")), _("ClosestPoint"), -1));
		}

		il2cpp_field_get(PlayerEyes*, eyes, eyes_ptr);
		il2cpp_field_get(PlayerInput*, input, input_ptr);
		il2cpp_field_get(BaseMountable*, mounted, mounted_ptr);
		il2cpp_field_get(float, last_sent_tick_time, last_sent_tick_time_ptr);
		il2cpp_field_get(int, flags, flags_ptr);
		il2cpp_field_set(int, set_flags, flags_ptr);
		il2cpp_field_get(float, last_headshot_sound_time, last_headshot_sound_time_ptr);
		il2cpp_field_set(float, set_last_headshot_sound_time, last_headshot_sound_time_ptr);
		il2cpp_field_get(unsigned int, active_item, active_item_ptr);
		il2cpp_field_get(unsigned long, user_id, user_id_ptr);
		il2cpp_field_get(PlayerModel*, model, model_ptr);
		il2cpp_field_get(ModelState*, model_state, model_state_ptr);
		il2cpp_field_get(PlayerInventory*, inventory, inventory_ptr);
		il2cpp_field_get(PlayerWalkMovement*, walk_movement, walk_movement_ptr);
		il2cpp_field_get(PlayerTeam*, client_team, client_team_ptr);
		il2cpp_static_field_get(PlayerTick*, last_sent_tick, last_sent_tick_ptr);
		il2cpp_field_get(unity::c_string*, display_name, display_name_ptr);
		il2cpp_field_get(unity::CapsuleCollider*, collider, collider_ptr);
		il2cpp_field_get(uintptr_t, metabolism, metabolism_ptr);
		il2cpp_field_get(int, selected_view_mode, selected_view_mode_ptr);
		il2cpp_field_set(int, set_selected_view_mode, selected_view_mode_ptr);
		il2cpp_field_get(float, clothing_move_speed_reduction, clothing_move_speed_reduction_ptr);
		il2cpp_field_get(bool, frozen, frozen_ptr);
		il2cpp_field_set(bool, set_frozen, frozen_ptr);
		il2cpp_field_set(float, set_client_tick_interval, client_tick_interval_ptr);
		il2cpp_field_set(float, set_clothing_blocks_aiming, clothing_blocks_aiming_ptr);
		il2cpp_field_set(float, set_clothing_move_speed_reduction, clothing_move_speed_reduction_ptr);

		static unity::c_list_dictionary* visible_player_list() {
			return il2cpp::get_static_field<unity::c_list_dictionary*>(visible_player_list_ptr);
		}

		inline void remove_flag(int flag) {
			auto player_flags = flags();
			player_flags &= ~flag;
			set_flags(player_flags);
		}

		inline void add_flag(int flag) {
			auto player_flags = flags();
			set_flags(player_flags |= flag);
		}

		inline bool has_flag(int flag) {
			auto player_flags = flags();
			return (player_flags & flag);
		}

		inline float radius() {
			return safe.call<float>(get_radius_ptr, this);
		}

		inline vec3_t pivot_point() {
			return safe.call<vec3_t>(pivot_point_ptr, this);
		}

		inline float height(bool ducked) {
			return safe.call<float>(get_height_ptr, this, ducked);
		}

		inline float jump_height() {
			return safe.call<float>(get_jump_height_ptr, this);
		}

		inline bool is_ducked() {
			return safe.call<bool>(is_ducked_ptr, this);
		}

		inline bool is_swimming() {
			return safe.call<bool>(is_swimming_ptr, this);
		}

		inline bool is_running() {
			return safe.call<bool>(is_running_ptr, this);
		}

		inline bool is_crawling() {
			return safe.call<bool>(is_crawling_ptr, this);
		}

		inline bool on_ladder() {
			return safe.call<bool>(on_ladder_ptr, this);
		}

		inline void on_land(float f_velocity) {
			safe.call<void>(on_land_ptr, this, f_velocity);
		}

		inline HeldEntity* held_entity() {
			return safe.call<HeldEntity*>(get_held_entity_ptr, this);
		}

		inline void send_client_tick() {
			safe.call<void>(send_client_tick_ptr, this);
		}

		struct obb_t {
		public:
			quat_t rotation; // 0x0
			vec3_t position; // 0x10
			vec3_t extents; // 0x1C
			vec3_t forward; // 0x28
			vec3_t right; // 0x34
			vec3_t up; // 0x40

			vec3_t closest_point(vec3_t target) {
				bool flag = false;
				bool flag2 = false;
				bool flag3 = false;

				vec3_t vector = position;
				vec3_t lhs = target - position;

				float num = lhs.dot(right);
				if (num > extents.x) {
					vector += right * extents.x;
				}
				else if (num < -extents.x) {
					vector -= right * extents.x;
				}
				else {
					flag = true;
					vector += right * num;
				}

				float num2 = lhs.dot(up);
				if (num2 > extents.y) {
					vector += up * extents.y;
				}
				else if (num2 < -extents.y) {
					vector -= up * extents.y;
				}
				else {
					flag2 = true;
					vector += up * num2;
				}

				float num3 = lhs.dot(forward);
				if (num3 > extents.z) {
					vector += forward * extents.z;
				}
				else if (num3 < -extents.z) {
					vector -= forward * extents.z;
				}
				else {
					flag3 = true;
					vector += forward * num3;
				}

				if (flag && flag2 && flag3)
					return target;

				return vector;
			}

			float distance_to(vec3_t pos) {
				return pos.distance_to(closest_point(pos));
			}
		};

		inline bounds_t bounds() {
			return safe.call<bounds_t>(get_bounds_ptr, this);
		}

		inline obb_t world_space_bounds() {
			return safe.call<obb_t>(world_space_bounds_ptr, this);
		}

		inline bool is_knocked() {
			return flags() & enums::e_flag::wounded;
		}

		inline bool is_sleeping() {
			return flags() & enums::e_flag::sleeping;
		}

		inline bool is_teammate() {
			const auto& local_team = local_player->client_team();
			if (!memory::is_valid(local_team))
				return false;

			const auto& members = local_team->members();
			if (!memory::is_valid(members))
				return false;

			const auto& steam_id = user_id();

			for (auto i = 0u; i < members->count(); ++i) { 
				const auto& member = members->value(i);
				if (!memory::is_valid(member))
					continue;

				if (member->user_id() == steam_id)
					return true;
			}

			return false;
		}

		inline float speed(float running, float ducking, float crawling) {
			return safe.call<float>(get_speed_ptr, this, running, ducking, crawling);
		}

		inline float max_speed() {
			return safe.call<float>(get_max_speed_ptr, this);
		}

		inline float max_velocity() {
			return safe.call<float>(max_velocity_ptr, this);
		}

		inline float bounds_padding() {
			return safe.call<float>(bounds_padding_ptr, this);
		}

		inline vec3_t mount_velocity() {
			return safe.call<vec3_t>(get_mount_velocity_ptr, this);
		}

		inline void on_view_mode_changed() {
			safe.call<void>(on_view_mode_changed_ptr, this);
		}

		inline void send_projectile_update(protobuf::PlayerProjectileUpdate* player_projectile_update) {
			safe.call<void>(send_projectile_update_ptr, this, player_projectile_update);
		}

		inline vec3_t closest_point(vec3_t position) {
			return safe.call<vec3_t>(closest_point_ptr, this, position);
		}

		inline bool is_local_player() {
			const auto& _model = model();
			if (!memory::is_valid(_model))
				return false;

			return _model->is_local_player();
		}

		inline Item* item() {
			const auto& active_uid = active_item();
			if (!active_uid)
				return nullptr;

			const auto& inv = inventory();
			if (!memory::is_valid(inv))
				return nullptr;

			const auto& container_belt = inv->containerBelt();
			if (!memory::is_valid(container_belt))
				return nullptr;

			const auto& item_list = container_belt->item_list();
			if (!memory::is_valid(item_list))
				return nullptr;

			const auto& count = item_list->count();

			for (auto i = 0u; i < count; ++i) {
				const auto& item = item_list->value(i);
				if (!memory::is_valid(item))
					continue;

				if (active_uid == item->uid())
					return item;
			}

			return nullptr;
		}

		inline void set_visible_state(bool state) {
			info.visible_states[this] = state;
		}

		inline void set_player_states(enums::e_player_state state) {
			info.player_states[this] = state;
		}

		inline auto player_state()
		{
			if (!info.player_states.contains(this))
				return enums::e_player_state::inside;

			return info.player_states[this];
		}

		inline bool is_visible() {
			if (!info.visible_states.contains(this))
				return false;

			return info.visible_states[this];
		}

		inline unity::Transform* bone_transform(int bone)
		{
			const auto& model = entity_model();
			if (!memory::is_valid(model))
				return { };

			const auto& bone_transforms = model->bone_transforms();
			if (!memory::is_valid(bone_transforms))
				return { };

			return bone_transforms->get(bone);
		}

		inline void set_bone_positions()
		{
			for (auto& bone : bones)
			{
				auto& player = info.bone_positions[this];
				player[bone] = bone_transform(bone)->position();
			}
		}

		inline vec3_t bone_position(enums::e_bone bone)
		{
			if (!info.bone_positions.contains(this))
				return { };

			if (!info.bone_positions[this].contains(bone))
				return { };

			return info.bone_positions[this][bone];
		}
	};
}
