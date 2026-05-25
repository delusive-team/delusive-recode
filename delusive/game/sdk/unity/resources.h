#pragma once
#include "../../il2cpp/il2cpp.h"
#include "array.h"

namespace unity {
	class Resources {
	private:
		static inline uintptr_t find_objects_of_type_all_ptr = 0;

	public:
		static void initialize() {
			auto klass = il2cpp::get_class(_("UnityEngine"), _("Resources"));
			find_objects_of_type_all_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("FindObjectsOfTypeAll"), 1));
		}

		template<typename T>
		static c_array<T>* find_objects_of_type_all(uintptr_t type) {
			return safe.call<c_array<T>*>(find_objects_of_type_all_ptr, type);
		}
	};
}
