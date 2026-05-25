#pragma once
#include "../il2cpp/il2cpp.h"
#include "base_combat_entity.h"

namespace sdk {
	class BaseMountable : public BaseCombatEntity {
	private:
		static inline uintptr_t can_wield_items_ptr = 0;

	public:
		static void initialize() {
			il2cpp_field_offset(can_wield_items_ptr, _(""), _("BaseMountable"), _("canWieldItems"));
		}

		il2cpp_field_set(bool, set_can_wield_items, can_wield_items_ptr);
	};
}
