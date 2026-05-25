#pragma once
#include "../il2cpp/il2cpp.h"
#include "base_entity.h"

namespace sdk {
	class Item; 

	class WorldItem : public BaseEntity {
	private:
		static inline uintptr_t item_ptr = 0;

	public:
		static void initialize() {
			il2cpp_field_offset(item_ptr, _(""), _("WorldItem"), _("item"));
		}

		il2cpp_field_get(Item*, item, item_ptr);
	};
}
