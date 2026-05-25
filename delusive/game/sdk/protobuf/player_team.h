#pragma once
#include "../../il2cpp/il2cpp.h"
#include "../unity/list.h"
#include "../unity/string.h"

namespace protobuf {
	class TeamMember {
	private:
		static inline uintptr_t user_id_ptr = 0;

	public:
		static void initialize() {
			il2cpp_field_offset_nested(user_id_ptr, _("ProtoBuf"), _("PlayerTeam"), _("TeamMember"), _("userID"));
		}

		il2cpp_field_get(unsigned long long, user_id, user_id_ptr);
	};

	class PlayerTeam {
	private:
		static inline uintptr_t members_ptr = 0;
		static inline uintptr_t team_id_ptr = 0;
		static inline uintptr_t team_name_ptr = 0;
		static inline uintptr_t team_leader_ptr = 0;

	public:
		static void initialize() {
			il2cpp_field_offset(members_ptr, _("ProtoBuf"), _("PlayerTeam"), _("members"));
			il2cpp_field_offset(team_id_ptr, _("ProtoBuf"), _("PlayerTeam"), _("teamID"));
			il2cpp_field_offset(team_name_ptr, _("ProtoBuf"), _("PlayerTeam"), _("teamName"));
			il2cpp_field_offset(team_leader_ptr, _("ProtoBuf"), _("PlayerTeam"), _("teamLeader"));
		}

		il2cpp_field_get(unity::c_list<TeamMember*>*, members, members_ptr);
		il2cpp_field_get(unsigned long long, team_id, team_id_ptr);
		il2cpp_field_get(unity::c_string*, team_name, team_name_ptr);
		il2cpp_field_get(unsigned long long, team_leader, team_leader_ptr);
	};
}
