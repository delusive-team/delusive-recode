#pragma once
#include "../il2cpp/il2cpp.h"
#include "math/vector.h"
#include "unity/transform.h"
#include "unity/component.h"
#include "unity/game_object.h"
#include "unity/string.h"
#include "unity/ray.h"
#include "protobuf/attack.h"

namespace sdk {
	class BaseEntity;
	class BasePlayer;

	class HitTest {
	private:
		// Fields
		static inline uintptr_t type_ptr = 0;
		static inline uintptr_t radius_ptr = 0;
		static inline uintptr_t did_hit_ptr = 0;
		static inline uintptr_t best_hit_ptr = 0;
		static inline uintptr_t collider_ptr = 0;
		static inline uintptr_t hit_part_ptr = 0;
		static inline uintptr_t hit_point_ptr = 0;
		static inline uintptr_t hit_normal_ptr = 0;
		static inline uintptr_t hit_entity_ptr = 0;
		static inline uintptr_t attack_ray_ptr = 0;
		static inline uintptr_t game_object_ptr = 0;
		static inline uintptr_t forgiveness_ptr = 0;
		static inline uintptr_t hit_material_ptr = 0;
		static inline uintptr_t hit_distance_ptr = 0;
		static inline uintptr_t max_distance_ptr = 0;
		static inline uintptr_t ignore_entity_ptr = 0;
		static inline uintptr_t hit_transform_ptr = 0;
		static inline uintptr_t damage_properties_ptr = 0;

		// Methods
		static inline uintptr_t hit_point_world_ptr = 0;
		static inline uintptr_t hit_normal_world_ptr = 0;

	public:
		static void initialize() {
			auto klass = il2cpp::get_class(_(""), _("HitTest"));

			il2cpp_field_offset(type_ptr, _(""), _("HitTest"), _("type"));
			il2cpp_field_offset(radius_ptr, _(""), _("HitTest"), _("Radius"));
			il2cpp_field_offset(did_hit_ptr, _(""), _("HitTest"), _("DidHit"));
			il2cpp_field_offset(best_hit_ptr, _(""), _("HitTest"), _("BestHit"));
			il2cpp_field_offset(collider_ptr, _(""), _("HitTest"), _("collider"));
			il2cpp_field_offset(hit_part_ptr, _(""), _("HitTest"), _("HitPart"));
			il2cpp_field_offset(hit_point_ptr, _(""), _("HitTest"), _("HitPoint"));
			il2cpp_field_offset(hit_normal_ptr, _(""), _("HitTest"), _("HitNormal"));
			il2cpp_field_offset(hit_entity_ptr, _(""), _("HitTest"), _("HitEntity"));
			il2cpp_field_offset(attack_ray_ptr, _(""), _("HitTest"), _("AttackRay"));
			il2cpp_field_offset(game_object_ptr, _(""), _("HitTest"), _("gameObject"));
			il2cpp_field_offset(forgiveness_ptr, _(""), _("HitTest"), _("Forgiveness"));
			il2cpp_field_offset(hit_material_ptr, _(""), _("HitTest"), _("HitMaterial"));
			il2cpp_field_offset(hit_distance_ptr, _(""), _("HitTest"), _("HitDistance"));
			il2cpp_field_offset(max_distance_ptr, _(""), _("HitTest"), _("MaxDistance"));
			il2cpp_field_offset(ignore_entity_ptr, _(""), _("HitTest"), _("ignoreEntity"));
			il2cpp_field_offset(hit_transform_ptr, _(""), _("HitTest"), _("HitTransform"));
			il2cpp_field_offset(damage_properties_ptr, _(""), _("HitTest"), _("damageProperties"));

			hit_point_world_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("HitPointWorld"), -1));
			hit_normal_world_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("HitNormalWorld"), -1));
		}

		il2cpp_field_get(BaseEntity*, hit_entity, hit_entity_ptr);
		il2cpp_field_get(vec3_t, hit_point, hit_point_ptr);
		il2cpp_field_get(vec3_t, hit_normal, hit_normal_ptr);
		il2cpp_field_get(unity::Transform*, hit_transform, hit_transform_ptr);

		il2cpp_field_set(int, set_type, type_ptr);
		il2cpp_field_set(float, set_radius, radius_ptr);
		il2cpp_field_set(bool, set_did_hit, did_hit_ptr);
		il2cpp_field_set(bool, set_best_hit, best_hit_ptr);
		il2cpp_field_set(float, set_forgiveness, forgiveness_ptr);
		il2cpp_field_set(vec3_t, set_hit_point, hit_point_ptr);
		il2cpp_field_set(vec3_t, set_hit_normal, hit_normal_ptr);
		il2cpp_field_set(unity::Component*, set_collider, collider_ptr);
		il2cpp_field_set(unity::GameObject*, set_game_object, game_object_ptr);
		il2cpp_field_set(unity::ray_t, set_attack_ray, attack_ray_ptr);
		il2cpp_field_set(float, set_hit_distance, hit_distance_ptr);
		il2cpp_field_set(float, set_max_distance, max_distance_ptr);
		il2cpp_field_set(unity::c_string*, set_hit_material, hit_material_ptr);
		il2cpp_field_set(unsigned int, set_hit_part, hit_part_ptr);
		il2cpp_field_set(BaseEntity*, set_hit_entity, hit_entity_ptr);
		il2cpp_field_set(BaseEntity*, set_ignore_entity, ignore_entity_ptr);
		il2cpp_field_set(unity::Transform*, set_hit_transform, hit_transform_ptr);
		il2cpp_field_set(uintptr_t, set_damage_properties, damage_properties_ptr);

		inline vec3_t hit_point_world() {
			return safe.call<vec3_t>(hit_point_world_ptr, this);
		}

		inline vec3_t hit_normal_world() {
			return safe.call<vec3_t>(hit_normal_world_ptr, this);
		}
	};
}
