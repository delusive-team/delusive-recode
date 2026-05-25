#pragma once
#include "../../il2cpp/il2cpp.h"

namespace convar {
	class Input {
	private:
		static inline uintptr_t sprinting_ptr = 0;

	public:
		static void initialize() {
			il2cpp_static_field(sprinting_ptr, _("ConVar"), _("Input"), _("alwayssprint"));
		}

		static void set_sprinting(bool value) {
			il2cpp::set_static_field<bool>(sprinting_ptr, value);
		}
	};
}
