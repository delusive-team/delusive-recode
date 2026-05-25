#pragma once
#include "../../il2cpp/il2cpp.h"

namespace convar {
	class Graphics {
	private:
		static inline uintptr_t fov_ptr = 0;

	public:
		static void initialize() {
			il2cpp_static_field(fov_ptr, _("ConVar"), _("Graphics"), _("_fov"));
		}

		static void set_fov(float value) {
			il2cpp::set_static_field<float>(fov_ptr, value);
		}
	};
}