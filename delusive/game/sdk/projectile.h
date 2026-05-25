#pragma once
#include "../il2cpp/il2cpp.h"
#include "unity/component.h"
#include "math/vector.h"

namespace sdk {
	class BasePlayer;
	class HitTest;
	class HeldEntity;

	class Projectile : public unity::Component {
	public:
		static constexpr float moveDeltaTime = 0.03125f;
		static constexpr float lifeTime = 8.f;

	private:
		static inline uintptr_t drag_ptr = 0;
		static inline uintptr_t invisible_ptr = 0;
		static inline uintptr_t tumble_speed_ptr = 0;
		static inline uintptr_t create_decals_ptr = 0;
		static inline uintptr_t owner_ptr = 0;
		static inline uintptr_t integrity_ptr = 0;
		static inline uintptr_t hit_test_ptr = 0;
		static inline uintptr_t max_distance_ptr = 0;
		static inline uintptr_t launch_time_ptr = 0;
		static inline uintptr_t traveled_time_ptr = 0;
		static inline uintptr_t close_flyby_distance_ptr = 0;
		static inline uintptr_t projectile_id_ptr = 0;
		static inline uintptr_t gravity_modifier_ptr = 0;
		static inline uintptr_t initial_distance_ptr = 0;
		static inline uintptr_t penetration_power_ptr = 0;
		static inline uintptr_t traveled_distance_ptr = 0;
		static inline uintptr_t initial_velocity_ptr = 0;
		static inline uintptr_t current_velocity_ptr = 0;
		static inline uintptr_t previous_position_ptr = 0;
		static inline uintptr_t damage_properties_ptr = 0;
		static inline uintptr_t source_weapon_prefab_ptr = 0;
		static inline uintptr_t source_projectile_prefab_ptr = 0;

		static inline uintptr_t thickness_ptr = 0;
		static inline uintptr_t sent_position_ptr = 0;

		static inline uintptr_t do_hit_ptr = 0;
		static inline uintptr_t retire_ptr = 0;
		static inline uintptr_t do_movement_ptr = 0;
		static inline uintptr_t do_flyby_sound_ptr = 0;
		static inline uintptr_t set_effect_scale_ptr = 0;
		static inline uintptr_t do_velocity_update_ptr = 0;

	public:
		static void initialize() {
			auto klass = il2cpp::get_class(_(""), _("Projectile"));
			
			il2cpp_field_offset(drag_ptr, _(""), _("Projectile"), _("drag"));
			il2cpp_field_offset(invisible_ptr, _(""), _("Projectile"), _("invisible"));
			il2cpp_field_offset(tumble_speed_ptr, _(""), _("Projectile"), _("tumbleSpeed"));
			il2cpp_field_offset(create_decals_ptr, _(""), _("Projectile"), _("createDecals"));
			il2cpp_field_offset(owner_ptr, _(""), _("Projectile"), _("owner"));
			il2cpp_field_offset(integrity_ptr, _(""), _("Projectile"), _("integrity"));
			il2cpp_field_offset(hit_test_ptr, _(""), _("Projectile"), _("hitTest"));
			il2cpp_field_offset(max_distance_ptr, _(""), _("Projectile"), _("maxDistance"));
			il2cpp_field_offset(launch_time_ptr, _(""), _("Projectile"), _("launchTime"));
			il2cpp_field_offset(traveled_time_ptr, _(""), _("Projectile"), _("traveledTime"));
			il2cpp_field_offset(close_flyby_distance_ptr, _(""), _("Projectile"), _("closeFlybyDistance"));
			il2cpp_field_offset(projectile_id_ptr, _(""), _("Projectile"), _("projectileID"));
			il2cpp_field_offset(gravity_modifier_ptr, _(""), _("Projectile"), _("gravityModifier"));
			il2cpp_field_offset(initial_distance_ptr, _(""), _("Projectile"), _("initialDistance"));
			il2cpp_field_offset(penetration_power_ptr, _(""), _("Projectile"), _("penetrationPower"));
			il2cpp_field_offset(traveled_distance_ptr, _(""), _("Projectile"), _("traveledDistance"));
			il2cpp_field_offset(initial_velocity_ptr, _(""), _("Projectile"), _("initialVelocity"));
			il2cpp_field_offset(current_velocity_ptr, _(""), _("Projectile"), _("currentVelocity"));
			il2cpp_field_offset(previous_position_ptr, _(""), _("Projectile"), _("previousPosition"));
			il2cpp_field_offset(damage_properties_ptr, _(""), _("Projectile"), _("damageProperties"));
			il2cpp_field_offset(source_weapon_prefab_ptr, _(""), _("Projectile"), _("sourceWeaponPrefab"));
			il2cpp_field_offset(source_projectile_prefab_ptr, _(""), _("Projectile"), _("sourceProjectilePrefab"));

			il2cpp_field_offset(thickness_ptr, _(""), _("Projectile"), _("thickness"));
			il2cpp_field_offset(sent_position_ptr, _(""), _("Projectile"), _("sentPosition"));

			do_hit_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("DoHit"), 3));
			retire_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("Retire"), -1));
			do_movement_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("DoMovement"), 1));
			do_flyby_sound_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("DoFlybySound"), -1));
			set_effect_scale_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("SetEffectScale"), 1));
			do_velocity_update_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("DoVelocityUpdate"), 1));
		}

		il2cpp_field_get(float, drag, drag_ptr);
		il2cpp_field_get(bool, invisible, invisible_ptr);
		il2cpp_field_get(float, tumble_speed, tumble_speed_ptr);
		il2cpp_field_get(bool, create_decals, create_decals_ptr);
		il2cpp_field_get(BasePlayer*, owner, owner_ptr);
		il2cpp_field_get(float, integrity, integrity_ptr);
		il2cpp_field_get(HitTest*, hit_test, hit_test_ptr);
		il2cpp_field_get(float, max_distance, max_distance_ptr);
		il2cpp_field_get(float, launch_time, launch_time_ptr);
		il2cpp_field_get(float, traveled_time, traveled_time_ptr);
		il2cpp_field_get(float, close_flyby_distance, close_flyby_distance_ptr);
		il2cpp_field_get(int, projectile_id, projectile_id_ptr);
		il2cpp_field_get(float, gravity_modifier, gravity_modifier_ptr);
		il2cpp_field_get(float, initial_distance, initial_distance_ptr);
		il2cpp_field_get(float, penetration_power, penetration_power_ptr);
		il2cpp_field_get(float, traveled_distance, traveled_distance_ptr);
		il2cpp_field_get(vec3_t, initial_velocity, initial_velocity_ptr);
		il2cpp_field_get(vec3_t, current_velocity, current_velocity_ptr);
		il2cpp_field_get(vec3_t, previous_position, previous_position_ptr);
		il2cpp_field_get(uintptr_t, damage_properties, damage_properties_ptr);
		il2cpp_field_get(HeldEntity*, source_weapon_prefab, source_weapon_prefab_ptr);
		il2cpp_field_get(Projectile*, source_projectile_prefab, source_projectile_prefab_ptr);

		il2cpp_field_set(float, set_integrity, integrity_ptr);
		il2cpp_field_set(float, set_thickness, thickness_ptr);
		il2cpp_field_set(HitTest*, set_hit_test, hit_test_ptr);
		il2cpp_field_set(vec3_t, set_sent_position, sent_position_ptr);
		il2cpp_field_set(float, set_traveled_time, traveled_time_ptr);
		il2cpp_field_set(vec3_t, set_previous_position, previous_position_ptr);
		il2cpp_field_set(vec3_t, set_initial_velocity, initial_velocity_ptr);
		il2cpp_field_set(vec3_t, set_current_velocity, current_velocity_ptr);
		il2cpp_field_set(float, set_initial_distance, initial_distance_ptr);
		il2cpp_field_set(float, set_traveled_distance, traveled_distance_ptr);

		inline void do_movement(float delta_time) {
			safe.call<void>(do_movement_ptr, this, delta_time);
		}

		inline void do_velocity_update(float delta_time) {
			safe.call<void>(do_velocity_update_ptr, this, delta_time);
		}

		inline void do_flyby_sound() {
			safe.call<void>(do_flyby_sound_ptr, this);
		}

		inline float effect_scale() {
			if (invisible()) {
				return 0.f;
			}
			if (traveled_distance() < 8.f) {
				return 0.f;
			}
			return vec3_t().inverse_lerp(12.f, 20.f, current_velocity().magnitude());
		}

		inline void set_effect_scale(float scale) {
			safe.call<void>(set_effect_scale_ptr, this, scale);
		}

		inline bool do_hit(HitTest* test, vec3_t point, vec3_t normal) {
			return safe.call<bool>(do_hit_ptr, this, test, point, normal);
		}

		inline void retire() {
			safe.call<void>(retire_ptr, this);
		}
	};
}
