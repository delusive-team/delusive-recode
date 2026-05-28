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

		static void set_underwater_effect(bool value) {
			static auto klass = il2cpp::get_class(_("ConVar"), _("Admin"));
			static auto method = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("set_underwater_effect"), 1));
			if (method) {
				safe.call<void>(method, value);
			}
		}
	};
}
