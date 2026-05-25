#pragma once
#include "../../il2cpp/il2cpp.h"

namespace convar {
	class Admin {
	private:
		static inline uintptr_t admintime = 0;

	public:
		static void initialize() {
			il2cpp_static_field(admintime, _("ConVar"), _("Admin"), _("admintime"));
		}

		static void set_admintime(float value) {
			il2cpp::set_static_field<float>(admintime, value);
		}
	};
}
