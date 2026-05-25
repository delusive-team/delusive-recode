#pragma once
#include "../il2cpp/il2cpp.h"
#include "math/vector.h"

namespace sdk {
	class BaseMovement {
	private:
		static inline uintptr_t get_target_movement_ptr = 0;
		static inline uintptr_t set_target_movement_ptr = 0;

	public:
		static void initialize() {
			get_target_movement_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_(""), _("BaseMovement")), _("get_TargetMovement"), -1));
			set_target_movement_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_(""), _("BaseMovement")), _("set_TargetMovement"), 1));
		}

		inline vec3_t target_movement() {
			return safe.call<vec3_t>(get_target_movement_ptr, this);
		}

		inline void set_target_movement(vec3_t pos) {
			safe.call<void>(set_target_movement_ptr, this, pos);
		}
	};
}
