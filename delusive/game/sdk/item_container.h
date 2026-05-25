#pragma once
#include "../il2cpp/il2cpp.h"
#include "unity/list.h"

namespace sdk {
	class ItemContainer {
	private:
		static inline uintptr_t item_list_ptr = 0;

	public:
		static void initialize() {
			il2cpp_field_offset(item_list_ptr, _(""), _("ItemContainer"), _("itemList"));
		}

		il2cpp_field_get(unity::c_list<Item*>*, item_list, item_list_ptr);
	};
}
