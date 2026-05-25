#pragma once
#include "../il2cpp/il2cpp.h"
#include "base_view_model.h"

namespace sdk {
	class ViewModel {
	private:
		static inline uintptr_t instance_ptr = 0;

	public:
		static void initialize() {
			il2cpp_field_offset(instance_ptr, _(""), _("ViewModel"), _("instance"));
		}

		il2cpp_field_get(BaseViewModel*, instance, instance_ptr);
	};
}
