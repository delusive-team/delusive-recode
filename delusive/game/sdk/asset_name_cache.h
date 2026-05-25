#pragma once
#include "../il2cpp/il2cpp.h"
#include "unity/string.h"

namespace sdk {
	class AssetNameCache {
	private:
		static inline uintptr_t get_name_ptr = 0;

	public:
		static void initialize() {
			get_name_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_(""), _("AssetNameCache")), _("GetName"), 1));
		}

		static unity::c_string* name(uintptr_t material) {
			return safe.call<unity::c_string*>(get_name_ptr, material);
		}
	};
}
