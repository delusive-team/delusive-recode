#pragma once
#include "../il2cpp/il2cpp.h"

namespace sdk {
	class TOD_AmbientParameters {
	private:
		static inline uintptr_t saturation_ptr = 0;

	public:
		static void initialize() {
			il2cpp_field_offset(saturation_ptr, _(""), _("TOD_AmbientParameters"), _("Saturation"));
		}

		il2cpp_field_set(float, set_saturation, saturation_ptr);
	};
}
