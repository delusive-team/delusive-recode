#pragma once
#include "../../il2cpp/il2cpp.h"

namespace convar {
	class Culling {
	private:
		static inline uintptr_t entity_min_cull_dist_ptr = 0;

	public:
		static void initialize() {
			il2cpp_static_field(entity_min_cull_dist_ptr, _("ConVar"), _("Culling"), _("entityMinCullDist"));
		}

		static void set_entity_min_cull_dist(float value) {
			il2cpp::set_static_field<float>(entity_min_cull_dist_ptr, value);
		}
	};
}