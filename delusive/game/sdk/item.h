#pragma once
#include "../il2cpp/il2cpp.h"
#include "unity/behaviour.h"
#include "unity/sprite.h"
#include "item_definition.h"
#include "held_entity.h"
#include "recoil_properties.h"
#include <string>

namespace sdk {
	class Item : public unity::Behaviour {
	private:
		static inline uintptr_t uid_ptr = 0;
		static inline uintptr_t info_ptr = 0;
		static inline uintptr_t amount_ptr = 0;
		static inline uintptr_t remove_time_ptr = 0;
		static inline uintptr_t held_entity_ptr = 0;

		static inline uintptr_t get_icon_sprite_ptr = 0;

	public:
		static void initialize() {
			auto klass = il2cpp::get_class(_(""), _("Item"));

			il2cpp_field_offset(uid_ptr, _(""), _("Item"), _("uid"));
			il2cpp_field_offset(info_ptr, _(""), _("Item"), _("info"));
			il2cpp_field_offset(amount_ptr, _(""), _("Item"), _("amount"));
			il2cpp_field_offset(remove_time_ptr, _(""), _("Item"), _("removeTime"));
			il2cpp_field_offset(held_entity_ptr, _(""), _("Item"), _("heldEntity"));

			get_icon_sprite_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("get_iconSprite"), -1));
		}

		il2cpp_field_get(int, amount, amount_ptr);
		il2cpp_field_get(unsigned int, uid, uid_ptr);
		il2cpp_field_get(float, remove_time, remove_time_ptr);
		il2cpp_field_get(ItemDefinition*, info, info_ptr);
		il2cpp_field_get(HeldEntity*, held_entity, held_entity_ptr);

		inline unity::Sprite* icon_sprite() {
			return safe.call<unity::Sprite*>(get_icon_sprite_ptr, this);
		}

		inline bool is_weapon() {
			const bool b_is_weapon = !is_class(_("BaseMelee")) &&
				!is_class(_("ThrownWeapon")) &&
				!is_class(_("TorchWeapon"));
			if (!b_is_weapon)
				return false;

			const auto& held = held_entity();
			if (!memory::is_valid(held))
				return false;

			if (b_is_weapon && (
				held->is_class(_("BaseProjectile")) ||
				held->is_class(_("BowWeapon")) ||
				held->is_class(_("CompoundBowWeapon")) ||
				held->is_class(_("CrossbowWeapon")) ||
				held->is_class(_("FlintStrikeWeapon"))))
				return true;

			return false;
		}

		inline bool is_heal() {
			const auto& held = held_entity();
			if (!memory::is_valid(held))
				return false;

			return held->is_class(_("MedicalTool"));
		}

		inline bool is_melee() {
			const auto& held = held_entity();
			if (!memory::is_valid(held))
				return false;

			if (
				held->is_class(_("ThrownWeapon")) ||
				held->is_class(_("BaseMelee")) ||
				held->is_class(_("Jackhammer")) ||
				held->is_class(_("Paddle"))
				)
				return true;

			return false;
		}

		inline void set_block_sprint(bool state) {
			const auto& entity = held_entity();
			if (!memory::is_valid(entity))
				return;

			entity->set_block_sprint(state);
		}

		inline void set_attack_max_distance(float value) {
			const auto& entity = held_entity();
			if (!memory::is_valid(entity))
				return;

			entity->set_max_distance(value);
			entity->set_attack_radius(value);
		}

		inline void set_attack_ready(bool state) {
			const auto& item_name = info()->shortname()->string();
			if (item_name.find(_("bow.compound")) != std::string::npos
				|| item_name.find(_("cross")) != std::string::npos)
				return;

			const auto& entity = held_entity();
			if (!memory::is_valid(entity))
				return;

			entity->set_attack_ready(state);
		}

		inline void set_was_aiming(bool state) {
			const auto& item_name = info()->shortname()->string();
			if (item_name.find(_("bow.compound")) != std::string::npos
				|| item_name.find(_("cross")) != std::string::npos)
				return;

			const auto& entity = held_entity();
			if (!memory::is_valid(entity))
				return;

			entity->set_was_aiming(state);
		}

		inline void set_always_eoka(float value) {
			const auto& item_name = info()->shortname()->string();
			if (item_name.find(_("eoka")) == std::string::npos)
				return;

			const auto& entity = held_entity();
			if (!memory::is_valid(entity))
				return;

			entity->set_success_fraction(value);
		}

		inline void set_did_spark_this_frame(bool state) {
			const auto& item_name = info()->shortname()->string();
			if (item_name.find(_("eoka")) == std::string::npos)
				return;

			const auto& entity = held_entity();
			if (!memory::is_valid(entity))
				return;

			entity->set_did_spark_this_frame(state);
		}

		inline void set_repeat_delay(float value) {
			const auto& entity = held_entity();
			if (!memory::is_valid(entity))
				return;

			entity->set_repeat_delay(value);
		}

		inline void set_current_hold_progress(float value) {
			const auto& item_name = info()->shortname()->string();
			if (item_name.find(_("bow.compound")) == std::string::npos)
				return;

			const auto& entity = held_entity();
			if (!memory::is_valid(entity))
				return;

			entity->set_current_hold_progress(value);
		}

		inline void set_string_bonus_damage(float value) {
			const auto& item_name = info()->shortname()->string();
			if (item_name.find(_("bow.compound")) == std::string::npos)
				return;

			const auto& entity = held_entity();
			if (!memory::is_valid(entity))
				return;

			entity->set_string_bonus_damage(value);
		}

		inline void set_movement_penalty_ramp_up_time(float value) {
			const auto& item_name = info()->shortname()->string();
			if (item_name.find(_("bow.compound")) == std::string::npos)
				return;

			const auto& entity = held_entity();
			if (!memory::is_valid(entity))
				return;

			entity->set_movement_penalty_ramp_up_time(value);
		}

		inline void server_rpc(const char* rpc) {
			const auto& entity = held_entity();
			if (!memory::is_valid(entity))
				return;

			entity->server_rpc(rpc);
		}

		inline void send_signal_broadcast(enums::e_signal signal) {
			const auto& entity = held_entity();
			if (!memory::is_valid(entity))
				return;

			entity->send_signal_broadcast(signal);
		}

		inline void send_broadcast_projectile(enums::e_signal signal, const char* rpc) {
			const auto& entity = held_entity();
			if (!memory::is_valid(entity))
				return;

			entity->send_signal_broadcast(signal, rpc);
		}

		inline void set_automatic(bool state) {
			const auto& entity = held_entity();
			if (!memory::is_valid(entity))
				return;

			entity->set_automatic(state);
		}

		inline float reload_time() {
			const auto& entity = held_entity();
			if (!memory::is_valid(entity))
				return 0.f;

			return entity->reload_time();
		}

		inline void set_aim_sway(float value) {
			if (!is_weapon())
				return;

			const auto& entity = held_entity();
			if (!memory::is_valid(entity))
				return;

			entity->set_aim_sway(value);
		}

		inline void set_aim_sway_speed(float value) {
			if (!is_weapon())
				return;

			const auto& entity = held_entity();
			if (!memory::is_valid(entity))
				return;

			entity->set_aim_sway_speed(value);
		}

		inline void set_aim_cone(float value) {
			if (!is_weapon())
				return;

			const auto& entity = held_entity();
			if (!memory::is_valid(entity))
				return;

			entity->set_aim_cone(value);
		}

		inline void set_aim_penalty_recover_delay(float value) {
			if (!is_weapon())
				return;

			const auto& entity = held_entity();
			if (!memory::is_valid(entity))
				return;

			entity->set_aimcone_penalty_recover_delay(value);
		}

		inline void set_aim_penalty_recover_time(float value) {
			if (!is_weapon())
				return;

			const auto& entity = held_entity();
			if (!memory::is_valid(entity))
				return;

			entity->set_aimcone_penalty_recover_time(value);
		}

		inline void set_stance_penalty(float value) {
			if (!is_weapon())
				return;

			const auto& entity = held_entity();
			if (!memory::is_valid(entity))
				return;

			entity->set_stance_penalty(value);
		}

		inline void set_aim_cone_penalty_max(float value) {
			if (!is_weapon())
				return;

			const auto& entity = held_entity();
			if (!memory::is_valid(entity))
				return;

			entity->set_aim_cone_penalty_max(value);
		}

		inline void set_aim_cone_penalty_per_shot(float value) {
			if (!is_weapon())
				return;

			const auto& entity = held_entity();
			if (!memory::is_valid(entity))
				return;

			entity->set_aimcone_penalty_per_shot(value);
		}

		inline void set_stance_penalty_scale(float value) {
			if (!is_weapon())
				return;

			const auto& entity = held_entity();
			if (!memory::is_valid(entity))
				return;

			entity->set_stance_penalty_scale(value);
		}

		inline void set_hip_aim_cone(float value) {
			if (!is_weapon())
				return;

			const auto& entity = held_entity();
			if (!memory::is_valid(entity))
				return;

			entity->set_hip_aim_cone(value);
		}

		inline void set_aim_cone_penalty(float value) {
			if (!is_weapon())
				return;

			const auto& entity = held_entity();
			if (!memory::is_valid(entity))
				return;

			entity->set_aimcone_penalty(value);
		}

		inline void set_projectile_velocity_scale(float value) {
			const auto& entity = held_entity();
			if (!memory::is_valid(entity))
				return;

			entity->set_projectile_velocity_scale(value);
		}

		inline void set_distance_scale(float value) {
			const auto& entity = held_entity();
			if (!memory::is_valid(entity))
				return;

			entity->set_distance_scale(value);
		}
	};
}
