#pragma once
#include "../../il2cpp/il2cpp.h"

namespace unity {
	class Screen {
	private:
		static inline uintptr_t get_width_ptr = 0;
		static inline uintptr_t get_height_ptr = 0;

	public:
		static void initialize() {
			get_width_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Screen")), _("get_width"), -1));
			get_height_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Screen")), _("get_height"), -1));
		}

		static int width() {
			return safe.call<int>(get_width_ptr);
		}

		static int height() {
			return safe.call<int>(get_height_ptr);
		}
	};
}
