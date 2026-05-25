#pragma once
#include "../il2cpp/il2cpp.h"
#include "math/vector.h"

namespace sdk {
	class PlayerTick {
	private:
		static inline uintptr_t position_ptr = 0;

	public:
		static void initialize() {
			il2cpp_field_offset(position_ptr, _(""), _("PlayerTick"), _("position"));
		}

		il2cpp_field_get(vec3_t, position, position_ptr);
	};
}
