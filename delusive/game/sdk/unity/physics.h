#pragma once
#include "../../il2cpp/il2cpp.h"
#include "../enums/enums.h"
#include "../math/vector.h"
#include "ray.h"
#include "raycast_hit.h"

namespace unity {
	class Physics {
	private:
		static inline uintptr_t raycast_ptr = 0;
		static inline uintptr_t sphere_cast_ptr = 0;
		static inline uintptr_t get_gravity_ptr = 0;
		static inline uintptr_t check_capsule_ptr = 0;
		static inline uintptr_t ignore_layer_collision_ptr = 0;

	public:
		static void initialize() {
			raycast_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Physics")), _("Raycast"), 5));
			sphere_cast_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Physics")), _("SphereCast"), 7));
			get_gravity_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Physics")), _("get_gravity"), -1));
			check_capsule_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Physics")), _("CheckCapsule"), 5));
			
			ignore_layer_collision_ptr = il2cpp::resolve_icall(_("UnityEngine.Physics::IgnoreLayerCollision(System.Int32,System.Int32,System.Boolean)"));
		}

		static bool check_capsule(vec3_t start, vec3_t end, float radius, int layer_mask, enums::e_query_trigger_interaction query_trigger_interaction) {
			return safe.call<bool>(check_capsule_ptr, start, end, radius, layer_mask, query_trigger_interaction);
		}

		static void ignore_layer_collision(enums::e_layer_collision layer1, enums::e_layer_collision layer2, bool ignore) {
			safe.call<void>(ignore_layer_collision_ptr, static_cast<int>(layer1), static_cast<int>(layer2), ignore);
		}

		static bool sphere_cast(vec3_t origin, float radius, vec3_t direction, raycast_hit_t* hit_info, float max_distance, int layer_mask, int query_trigger_interaction) {
			return safe.call<bool>(sphere_cast_ptr, origin, radius, direction, hit_info, max_distance, layer_mask, query_trigger_interaction);
		}

		static bool raycast(ray_t a1, raycast_hit_t hit, float a3, int a4, enums::e_query_trigger_interaction query_trigger_interaction) {
			return safe.call<bool>(raycast_ptr, a1, hit, a3, a4, query_trigger_interaction);
		}

		static vec3_t gravity() {
			return safe.call<vec3_t>(get_gravity_ptr);
		}
	};
}
