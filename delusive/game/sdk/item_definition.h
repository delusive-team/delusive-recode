#pragma once
#include "../il2cpp/il2cpp.h"
#include "unity/component.h"
#include "unity/string.h"
#include "enums/enums.h"

namespace sdk {
	class TranslatePhrase;

	class ItemDefinition : public unity::Component {
	private:
		static inline uintptr_t itemid_ptr = 0;
		static inline uintptr_t quick_despawn_ptr = 0;
		static inline uintptr_t category_ptr = 0;
		static inline uintptr_t shortname_ptr = 0;
		static inline uintptr_t display_name_ptr = 0;

	public:
		static void initialize() {
			il2cpp_field_offset(itemid_ptr, _(""), _("ItemDefinition"), _("itemid"));
			il2cpp_field_offset(quick_despawn_ptr, _(""), _("ItemDefinition"), _("quickDespawn"));
			il2cpp_field_offset(category_ptr, _(""), _("ItemDefinition"), _("category"));
			il2cpp_field_offset(shortname_ptr, _(""), _("ItemDefinition"), _("shortname"));
			il2cpp_field_offset(display_name_ptr, _(""), _("ItemDefinition"), _("displayName"));
		}

		il2cpp_field_get(unsigned int, itemid, itemid_ptr);
		il2cpp_field_get(bool, quick_despawn, quick_despawn_ptr);
		il2cpp_field_get(enums::e_item_category, category, category_ptr);
		il2cpp_field_get(unity::c_string*, shortname, shortname_ptr);
		il2cpp_field_get(TranslatePhrase*, display_name, display_name_ptr);
	};
}
