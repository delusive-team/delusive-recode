#pragma once
#include "../il2cpp/il2cpp.h"

namespace sdk {
	class Animator {
	private:
		static inline uintptr_t set_speed_ptr = 0;

	public:
		static void initialize() {
			set_speed_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Animator")), _("set_speed"), 1));
		}

		inline void set_speed(float speed) {
			safe.call<void>(set_speed_ptr, this, speed);
		}
	};
}