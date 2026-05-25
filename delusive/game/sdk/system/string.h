#pragma once
#include "../../il2cpp/il2cpp.h"

namespace sys {
	class String {
	public:
		static inline uintptr_t contains_ptr = 0;

		static void initialize() {
			contains_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("System"), _("String")), _("Contains"), 1));
		}
	};
}
