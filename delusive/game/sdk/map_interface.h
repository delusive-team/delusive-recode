#pragma once
#include "../il2cpp/il2cpp.h"

namespace sdk {
	class MapInterface {
	private:
		static inline uintptr_t is_open_ptr = 0;

	public:
		static void initialize() {
			il2cpp_static_field(is_open_ptr, _(""), _("MapInterface"), _("IsOpen"));
		}

		static bool is_open() {
			return il2cpp::get_static_field<bool>(is_open_ptr);
		}
	};
}
