#pragma once
#include "../il2cpp/il2cpp.h"

namespace sdk {
	class TOD_WorldParameters {
	private:
		static inline uintptr_t latitude_ptr = 0;
		static inline uintptr_t longitude_ptr = 0;

	public:
		static void initialize() {
			il2cpp_field_offset(latitude_ptr, _(""), _("TOD_WorldParameters"), _("Latitude"));
			il2cpp_field_offset(longitude_ptr, _(""), _("TOD_WorldParameters"), _("Longitude"));
		}

		il2cpp_field_set(float, set_latitude, latitude_ptr);
		il2cpp_field_set(float, set_longitude, longitude_ptr);
	};
}
