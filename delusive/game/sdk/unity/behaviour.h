#pragma once
#include "../../il2cpp/il2cpp.h"
#include "component.h"

namespace unity {
	class Behaviour : public Component {
	private:
		static inline uintptr_t set_enabled_ptr = 0;
		static inline uintptr_t get_enabled_ptr = 0;

	public:
		static void initialize() {
			set_enabled_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Behaviour")), _("set_enabled"), 1));
			get_enabled_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Behaviour")), _("get_enabled"), -1));
		}

		inline bool enabled() {
			return safe.call<bool>(get_enabled_ptr, this);
		}

		inline void set_enabled(bool state) {
			safe.call<void>(set_enabled_ptr, this, state);
		}
	};
}
