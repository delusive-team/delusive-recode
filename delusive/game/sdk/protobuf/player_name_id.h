#pragma once
#include "../../il2cpp/il2cpp.h"
#include "../unity/string.h"

namespace protobuf {
	class PlayerNameID {
	private:
		static inline uintptr_t username_ptr = 0;

	public:
		static void initialize() {
			il2cpp_field_offset(username_ptr, _("ProtoBuf"), _("PlayerNameID"), _("username"));
		}

		il2cpp_field_get(unity::c_string*, username, username_ptr);
	};
}
