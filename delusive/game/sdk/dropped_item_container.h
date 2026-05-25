#pragma once
#include "../il2cpp/il2cpp.h"
#include "unity/string.h"

namespace sdk {
	class DroppedItemContainer {
	private:
		static inline uintptr_t player_steam_id_ptr = 0;
		static inline uintptr_t get_player_name_ptr = 0;

	public:
		static void initialize() {
			auto klass = il2cpp::get_class(_(""), _("DroppedItemContainer"));
			il2cpp_field_offset(player_steam_id_ptr, _(""), _("DroppedItemContainer"), _("playerSteamID"));
			get_player_name_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("get_playerName"), -1));
		}

		il2cpp_field_get(uint32_t, player_steam_id, player_steam_id_ptr);

		inline unity::c_string* player_name() {
			return safe.call<unity::c_string*>(get_player_name_ptr, this);
		}
	};
}
