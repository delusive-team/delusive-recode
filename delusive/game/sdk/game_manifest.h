#pragma once
#include "../il2cpp/il2cpp.h"
#include "unity/string.h"
#include "unity/game_object.h"

namespace sdk {
	class GameManifest {
	private:
		static inline uintptr_t guid_to_object_ptr = 0;

	public:
		static void initialize() {
			auto klass = il2cpp::get_class(_(""), _("GameManifest"));
			guid_to_object_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("GUIDToObject"), 1));
		}

		static inline unity::GameObject* guid_to_object(unity::c_string* guid) {
			return safe.call<unity::GameObject*>(guid_to_object_ptr, guid);
		}
	};
}
