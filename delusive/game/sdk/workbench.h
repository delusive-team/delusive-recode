#pragma once
#include "../il2cpp/il2cpp.h"

namespace sdk {
	class Workbench {
	private:
		static inline uintptr_t level_ptr = 0;

	public:
		static void initialize() {
			il2cpp_field_offset(level_ptr, _(""), _("Workbench"), _("Workbenchlevel"));
		}

		il2cpp_field_get(int, level, level_ptr);
	};
}
