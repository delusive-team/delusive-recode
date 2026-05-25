#pragma once
#include "../il2cpp/il2cpp.h"
#include "math/vector.h"

namespace sdk {
	class InputState; // Forward declaration

	class PlayerInput {
	private:
		static inline uintptr_t state_ptr = 0;
		static inline uintptr_t body_angles_ptr = 0;
		static inline uintptr_t recoil_angles_ptr = 0;

	public:
		static void initialize() {
			il2cpp_field_offset(state_ptr, _(""), _("PlayerInput"), _("state"));
			il2cpp_field_offset(body_angles_ptr, _(""), _("PlayerInput"), _("bodyAngles"));
			il2cpp_field_offset(recoil_angles_ptr, _(""), _("PlayerInput"), _("recoilAngles"));
		}

		il2cpp_field_get(InputState*, state, state_ptr);
		il2cpp_field_get(vec2_t, recoil_angles, recoil_angles_ptr);

		il2cpp_field_get(vec2_t, body_angles, body_angles_ptr);
		il2cpp_field_set(vec2_t, set_body_angles, body_angles_ptr);
	};
}
