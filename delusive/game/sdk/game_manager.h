#pragma once
#include "../il2cpp/il2cpp.h"

namespace sdk {
	class GameManager {
	private:
		static inline uintptr_t instance_ptr = 0;
		static inline uintptr_t create_prefab_ptr = 0;

	public:
		static void initialize() {
			auto klass = il2cpp::get_class(_(""), _("GameManager"));
			il2cpp_static_field(instance_ptr, _(""), _("GameManager"), _("client"));
			create_prefab_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("CreatePrefab"), 2));
		}

		static inline GameManager* instance() {
			return mem::read<GameManager*>(instance_ptr);
		}

		inline unity::GameObject* create_prefab(const char* prefab) {
			return safe.call<unity::GameObject*>(create_prefab_ptr, this, il2cpp::new_string(prefab), true);
		}
	};
}
