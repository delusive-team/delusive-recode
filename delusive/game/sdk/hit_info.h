#pragma once
#include "../il2cpp/il2cpp.h"
#include "math/vector.h"
#include "protobuf/attack.h"

namespace sdk {
	class BasePlayer;
	class HeldEntity;
	class Projectile;

	class HitInfo {
	private:
		// Fields
		static inline uintptr_t did_hit_ptr = 0;
		static inline uintptr_t hit_bone_ptr = 0;
		static inline uintptr_t do_decals_ptr = 0;
		static inline uintptr_t initiator_ptr = 0;
		static inline uintptr_t point_start_ptr = 0;
		static inline uintptr_t projectile_id_ptr = 0;
		static inline uintptr_t weapon_prefab_ptr = 0;
		static inline uintptr_t is_predicting_ptr = 0;
		static inline uintptr_t projectile_prefab_ptr = 0;
		static inline uintptr_t hit_position_world_ptr = 0;
		static inline uintptr_t projectile_distance_ptr = 0;
		static inline uintptr_t projectile_velocity_ptr = 0;

		// Methods
		static inline uintptr_t position_on_ray_ptr = 0;
		static inline uintptr_t load_from_attack_ptr = 0;

	public:
		static void initialize() {
			auto klass = il2cpp::get_class(_(""), _("HitInfo"));

			il2cpp_field_offset(did_hit_ptr, _(""), _("HitInfo"), _("DidHit"));
			il2cpp_field_offset(hit_bone_ptr, _(""), _("HitInfo"), _("HitBone"));
			il2cpp_field_offset(do_decals_ptr, _(""), _("HitInfo"), _("DoDecals"));
			il2cpp_field_offset(initiator_ptr, _(""), _("HitInfo"), _("Initiator"));
			il2cpp_field_offset(point_start_ptr, _(""), _("HitInfo"), _("PointStart"));
			il2cpp_field_offset(projectile_id_ptr, _(""), _("HitInfo"), _("ProjectileID"));
			il2cpp_field_offset(weapon_prefab_ptr, _(""), _("HitInfo"), _("WeaponPrefab"));
			il2cpp_field_offset(is_predicting_ptr, _(""), _("HitInfo"), _("IsPredicting"));
			il2cpp_field_offset(projectile_prefab_ptr, _(""), _("HitInfo"), _("ProjectilePrefab"));
			il2cpp_field_offset(hit_position_world_ptr, _(""), _("HitInfo"), _("HitPositionWorld"));
			il2cpp_field_offset(projectile_distance_ptr, _(""), _("HitInfo"), _("ProjectileDistance"));
			il2cpp_field_offset(projectile_velocity_ptr, _(""), _("HitInfo"), _("ProjectileVelocity"));

			position_on_ray_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("PositionOnRay"), 1));
			load_from_attack_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("LoadFromAttack"), 2));
		}

		il2cpp_field_get(BasePlayer*, initiator, initiator_ptr);
		il2cpp_field_get(bool, did_hit, did_hit_ptr);
		il2cpp_field_get(vec3_t, point_start, point_start_ptr);
		il2cpp_field_get(vec3_t, hit_position_world, hit_position_world_ptr);
		il2cpp_field_get(float, projectile_distance, projectile_distance_ptr);

		il2cpp_field_set(BasePlayer*, set_initiator, initiator_ptr);
		il2cpp_field_set(uint32_t, set_hit_bone, hit_bone_ptr);
		il2cpp_field_set(int, set_projectile_id, projectile_id_ptr);
		il2cpp_field_set(HeldEntity*, set_weapon_prefab, weapon_prefab_ptr);
		il2cpp_field_set(bool, set_is_predicting, is_predicting_ptr);
		il2cpp_field_set(bool, set_do_decals, do_decals_ptr);
		il2cpp_field_set(float, set_projectile_distance, projectile_distance_ptr);
		il2cpp_field_set(vec3_t, set_projectile_velocity, projectile_velocity_ptr);
		il2cpp_field_set(Projectile*, set_projectile_prefab, projectile_prefab_ptr);

		inline vec3_t position_on_ray(vec3_t pos) {
			return safe.call<vec3_t>(position_on_ray_ptr, this, pos);
		}

		inline void load_from_attack(protobuf::Attack* attack, bool state) {
			safe.call<void>(load_from_attack_ptr, this, attack, state);
		}
	};
}
