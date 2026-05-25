#pragma once
#include "../../il2cpp/il2cpp.h"
#include "object.h"

namespace unity {
    class Transform; // forward declaration
    class GameObject;

	class Component : public Object {
	private:
		static inline uintptr_t get_component_ptr = 0;
		static inline uintptr_t get_transform_ptr = 0;
		static inline uintptr_t get_game_object_ptr = 0;
		static inline uintptr_t get_components_in_children_ptr = 0;

	public:
		static void initialize() {
			get_component_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Component")), _("GetComponent"), 1));
			get_transform_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Component")), _("get_transform"), -1));
			get_game_object_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Component")), _("get_gameObject"), -1));
			get_components_in_children_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Component")), _("GetComponentsInChildren"), 1));
		}

		inline Transform* transform() {
			return safe.call<Transform*>(get_transform_ptr, this);
		}

		template <class T>
		inline T component(uintptr_t type) {
			return safe.call<T>(get_component_ptr, this, type);
		}

		template <class T>
		inline c_array<T>* components_in_children(uintptr_t type) {
			return safe.call<c_array<T>*>(get_components_in_children_ptr, this, type);
		}

		inline GameObject* game_object() {
			return safe.call<GameObject*>(get_game_object_ptr, this);
		}
	};
}
