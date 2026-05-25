#pragma once
#include "../il2cpp/il2cpp.h"
#include "unity/string.h"

namespace sdk {
	class LootableCorpse {
	private:
		static inline uintptr_t player_name_ptr = 0;

	public:
		static void initialize() {
			il2cpp_field_offset(player_name_ptr, _(""), _("LootableCorpse"), _("_playerName"));
		}

		il2cpp_field_get(unity::c_string*, player_name, player_name_ptr);
	};
}
