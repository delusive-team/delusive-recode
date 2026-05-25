#pragma once
#include "../il2cpp/il2cpp.h"
#include "unity/list_dictionary.h"

namespace sdk {
	class BasePlayer;

	class LocalPlayer {
	private:
		static inline uintptr_t get_entity_ptr = 0;

	public:
		static void initialize() {
			get_entity_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_(""), _("LocalPlayer")), _("get_Entity"), -1));
		}

		static inline BasePlayer* entity() {
			return safe.call<BasePlayer*>(get_entity_ptr);
		}
	};
}
