#pragma once
#include "../il2cpp/il2cpp.h"
#include "unity/string.h"

namespace sdk {
	class RandomUsernames {
	private:
		static inline uintptr_t get_ptr = 0;

	public:
		static void initialize() {
			get_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("Facepunch"), _("RandomUsernames")), _("Get"), 1));
		}

		static unity::c_string* get(unsigned v) {
			return safe.call<unity::c_string*>(get_ptr, v);
		}
	};

	class SteamPlatform {
	private:
		static inline uintptr_t set_user_name_ptr = 0;

	public:
		static void initialize() {
			set_user_name_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("Rust.Platform.Steam"), _("SteamPlatform")), _("set_UserName"), 1));
		}

		inline unity::c_string* set_username(unity::c_string* name) {
			return safe.call<unity::c_string*>(set_user_name_ptr, this, name);
		}
	};
}
