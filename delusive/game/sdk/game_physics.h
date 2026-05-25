#pragma once
#include "../il2cpp/il2cpp.h"
#include "math/vector.h"
#include "enums/enums.h"
#include "unity/raycast_hit.h"
#include "unity/ray.h"

namespace sdk {
	class GamePhysics {
	private:
		static inline uintptr_t trace_ptr = 0;
		static inline uintptr_t verify_ptr = 0;
		static inline uintptr_t line_of_sight_ptr = 0;
		static inline uintptr_t line_of_sight_radius_ptr = 0;

	public:
		static void initialize() {
			auto klass = il2cpp::get_class(_(""), _("GamePhysics"));
			trace_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("Trace"), 6));
			verify_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("Verify"), 2));
			line_of_sight_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("LineOfSight"), 4));
			line_of_sight_radius_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("LineOfSightRadius"), 5));
		}

		static inline bool is_visible(vec3_t start, vec3_t end, float p1 = 0.2f) {
			int mask = 1503731969;
			return safe.call<bool>(line_of_sight_radius_ptr, start, end, mask, p1, nullptr);
		}

		static inline bool line_of_sight(vec3_t p0, vec3_t p1, int layer_mask, void* ignore_entity = nullptr) {
			return safe.call<bool>(line_of_sight_ptr, p0, p1, layer_mask, ignore_entity);
		}

		static inline bool verify(unity::raycast_hit_t hit_info, void* ignore_entity = nullptr) {
			return safe.call<bool>(verify_ptr, hit_info, nullptr);
		}

		static inline bool trace(unity::ray_t ray, float radius, unity::raycast_hit_t hit_info, float max_distance, int layer_mask, enums::e_query_trigger_interaction trigger_interaction, void* ignore_entity = nullptr) {
			return safe.call<bool>(trace_ptr, ray, radius, hit_info, max_distance, layer_mask, trigger_interaction, ignore_entity);
		}
	};
}
