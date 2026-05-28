#pragma once
#include "../il2cpp/il2cpp.h"
#include "math/vector.h"

namespace sdk {
	class InputMessage {
	private:
		static inline uintptr_t aim_angles_ptr = 0;

	public:
		static void initialize() {
			il2cpp_field_offset(aim_angles_ptr, _(""), _("InputMessage"), _("aimAngles"));
		}

		il2cpp_field_get(vec3_t, aim_angles, aim_angles_ptr);
		il2cpp_field_set(vec3_t, set_aim_angles, aim_angles_ptr);
	};
}
