#pragma once
#include "../../il2cpp/il2cpp.h"
#include "component.h"
#include "../math/bounds.h"

namespace unity {
	class Collider : public Component {
	private:
		static inline uintptr_t get_bounds_injected_ptr = 0;

	public:
		static void initialize() {
			get_bounds_injected_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Collider")), _("get_bounds_Injected"), -1));
		}

		inline bounds_t bounds() {
			bounds_t result;
			safe.call<void>(get_bounds_injected_ptr, this, &result);
			return result;
		}
	};
}
