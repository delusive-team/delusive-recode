#pragma once
#include "../il2cpp/il2cpp.h"

namespace sdk {
	class ItemContainer; // Forward declaration

	class PlayerInventory {
	private:
		static inline uintptr_t container_belt_ptr = 0;

	public:
		static void initialize() {
			il2cpp_field_offset(container_belt_ptr, _(""), _("PlayerInventory"), _("containerBelt"));
		}

		il2cpp_field_get(ItemContainer*, containerBelt, container_belt_ptr);
	};
}
