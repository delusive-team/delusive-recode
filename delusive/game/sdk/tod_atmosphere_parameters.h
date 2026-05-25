#pragma once
#include "../il2cpp/il2cpp.h"

namespace sdk {
	class TOD_AtmosphereParameters {
	private:
		static inline uintptr_t rayleigh_multiplier_ptr = 0;

	public:
		static void initialize() {
			il2cpp_field_offset(rayleigh_multiplier_ptr, _(""), _("TOD_AtmosphereParameters"), _("RayleighMultiplier"));
		}

		il2cpp_field_set(float, set_rayleigh_multiplier, rayleigh_multiplier_ptr);
	};
}
