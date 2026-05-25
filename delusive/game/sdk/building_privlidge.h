#pragma once
#include "../il2cpp/il2cpp.h"
#include "unity/list.h"
#include "protobuf/player_name_id.h"

namespace sdk {
	class BuildingPrivlidge {
	private:
		static inline uintptr_t authorized_players_ptr = 0;

	public:
		static void initialize() {
			il2cpp_field_offset(authorized_players_ptr, _(""), _("BuildingPrivlidge"), _("authorizedPlayers"));
		}

		il2cpp_field_get(unity::c_list<protobuf::PlayerNameID*>*, authorized_players, authorized_players_ptr);
	};
}
