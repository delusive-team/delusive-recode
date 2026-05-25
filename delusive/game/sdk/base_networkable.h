#pragma once
#include "../il2cpp/il2cpp.h"
#include "unity/component.h"
#include "unity/string.h"
#include "entity_realm.h"

namespace sdk {
	class BaseNetworkable : public unity::Component {
	private:
		static inline uintptr_t client_entities_ptr = 0;
		static inline uintptr_t has_parent_ptr = 0;
		static inline uintptr_t get_short_prefab_name_ptr = 0;

	public:
		static void initialize() {
			auto klass = il2cpp::get_class(_(""), _("BaseNetworkable"));
			il2cpp_static_field(client_entities_ptr, _(""), _("BaseNetworkable"), _("clientEntities"));
			has_parent_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("HasParent"), -1));
			get_short_prefab_name_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("get_ShortPrefabName"), -1));
		}

		static inline EntityRealm* client_entities() {
			return il2cpp::get_static_field<EntityRealm*>(client_entities_ptr);
		}

		inline unity::c_string* prefab() {
			return safe.call<unity::c_string*>(get_short_prefab_name_ptr, this);
		}

		inline bool has_parent() {
			return safe.call<bool>(has_parent_ptr, this);
		}
	};
}
