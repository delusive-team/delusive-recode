#pragma once
#include "../il2cpp/il2cpp.h"
#include "unity/list.h"

namespace sdk {
	class PlayerTeamMember {
	private:
		static inline uintptr_t user_id_ptr = 0;

	public:
		static void initialize() {
			il2cpp_field_offset(user_id_ptr, _(""), _("PlayerTeam"), _("userID"));
			if (!user_id_ptr) 
				user_id_ptr = 0x20;
		}

		inline unsigned long user_id() {
			return mem::read<unsigned long>((uintptr_t)this + user_id_ptr);
		}
	};

	class PlayerTeam {
	private:
		static inline uintptr_t members_ptr = 0;

	public:
		static void initialize() {
			il2cpp_field_offset(members_ptr, _(""), _("PlayerTeam"), _("members"));
			if (!members_ptr)
				members_ptr = 0x30;
		}

		inline unity::c_list<PlayerTeamMember*>* members() {
			return mem::read<unity::c_list<PlayerTeamMember*>*>((uintptr_t)this + members_ptr);
		}
	};
}
