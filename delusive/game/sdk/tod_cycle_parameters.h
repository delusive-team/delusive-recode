#pragma once
#include "../il2cpp/il2cpp.h"

namespace sdk {
	class TOD_CycleParameters {
	private:
		static inline uintptr_t hour_ptr = 0;

	public:
		static void initialize() {
			il2cpp_field_offset(hour_ptr, _(""), _("TOD_CycleParameters"), _("Hour"));
		}

		il2cpp_field_get(float, hour, hour_ptr);
		il2cpp_field_set(float, set_hour, hour_ptr);
	};
}
