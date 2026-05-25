#pragma once
#include "../il2cpp/il2cpp.h"
#include "held_entity.h"

namespace sdk {
	class BaseMelee : public HeldEntity {
	private:
		static inline uintptr_t attack_radius_ptr = 0;
		static inline uintptr_t max_distance_ptr = 0;

	public:
		static void initialize() {
			il2cpp_field_offset(attack_radius_ptr, _(""), _("BaseMelee"), _("attackRadius"));
			il2cpp_field_offset(max_distance_ptr, _(""), _("BaseMelee"), _("maxDistance"));
		}

		il2cpp_field_get(float, attack_radius, attack_radius_ptr);
		il2cpp_field_set(float, set_attack_radius, attack_radius_ptr);

		il2cpp_field_get(float, max_distance, max_distance_ptr);
		il2cpp_field_set(float, set_max_distance, max_distance_ptr);
	};
}
