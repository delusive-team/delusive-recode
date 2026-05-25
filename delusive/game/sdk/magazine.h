#pragma once
#include "../il2cpp/il2cpp.h"

namespace sdk {
	class ItemDefinition;

	class Magazine {
	private:
		static inline uintptr_t capacity_ptr = 0;
		static inline uintptr_t contents_ptr = 0;
		static inline uintptr_t ammo_type_ptr = 0;

	public:
		static void initialize() {
			il2cpp_field_offset_nested(capacity_ptr, _(""), _("BaseProjectile"), _("Magazine"), _("capacity"));
			il2cpp_field_offset_nested(contents_ptr, _(""), _("BaseProjectile"), _("Magazine"), _("contents"));
			il2cpp_field_offset_nested(ammo_type_ptr, _(""), _("BaseProjectile"), _("Magazine"), _("ammoType"));
		}

		il2cpp_field_get(int, capacity, capacity_ptr);
		il2cpp_field_set(int, set_capacity, capacity_ptr);
		il2cpp_field_get(int, contents, contents_ptr);
		il2cpp_field_set(int, set_contents, contents_ptr);
		il2cpp_field_get(ItemDefinition*, ammo_type, ammo_type_ptr);
	};
}
