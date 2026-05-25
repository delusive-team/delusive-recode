#pragma once
#include "../../il2cpp/il2cpp.h"
#include "../math/vector.h"

namespace unity {
	class RigidBody {
	private:
		static inline uintptr_t get_velocity_ptr = 0;
		static inline uintptr_t set_velocity_ptr = 0;

	public:
		static void initialize() {
			get_velocity_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Rigidbody")), _("get_velocity"), -1));
			set_velocity_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Rigidbody")), _("set_velocity"), 1));
		}

		inline vec3_t velocity() {
			return safe.call<vec3_t>(get_velocity_ptr, this);
		}

		inline void set_velocity(vec3_t velocity) {
			safe.call<void>(set_velocity_ptr, this, velocity);
		}
	};
}
