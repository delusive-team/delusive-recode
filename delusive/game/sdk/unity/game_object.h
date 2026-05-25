#pragma once
#include "../../il2cpp/il2cpp.h"
#include "object.h"

namespace unity {
    class Transform;

	class GameObject : public Object {
	private:
		static inline uintptr_t find_ptr = 0;
		static inline uintptr_t set_active_ptr = 0;
		static inline uintptr_t get_component_ptr = 0;
		static inline uintptr_t get_transform_ptr = 0;
		static inline uintptr_t get_component_in_children_ptr = 0;
		static inline uintptr_t get_components_in_children_ptr = 0;

	public:
		static void initialize() {
			find_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("GameObject")), _("Find"), 1));
			set_active_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("GameObject")), _("SetActive"), 1));
			get_component_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("GameObject")), _("GetComponent"), 1));
			get_transform_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("GameObject")), _("get_transform"), -1));
			get_component_in_children_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("GameObject")), _("GetComponentInChildren"), 2));
			get_components_in_children_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("GameObject")), _("GetComponentsInChildren"), -1));
		}

		inline Transform* transform() {
			return safe.call<Transform*>(get_transform_ptr, this);
		}

		inline void set_active(bool state) {
			safe.call<void>(set_active_ptr, this, state);
		}

		template <class T>
		inline T component(uintptr_t type) {
			return safe.call<T>(get_component_ptr, this, type);
		}

		template <class T>
		inline T component_in_children(uintptr_t type) {
			return safe.call<T>(get_component_in_children_ptr, this, type, false);
		}

		template <class T>
		inline c_array<T>* components_in_children(uintptr_t type) {
			return safe.call<c_array<T>*>(get_components_in_children_ptr, this, type);
		}

		static GameObject* find(const char* name) {
			return safe.call<GameObject*>(find_ptr, il2cpp::new_string(name));
		}

		template<class T>
		static T get(const char* name) {
			const auto& game_obj = find(name);
			if (!memory::is_valid(game_obj))
				return { };

			return mem::read_chain<T>(reinterpret_cast<uintptr_t>(game_obj), { 0x10, 0x30, 0x18, 0x28 });
		}
	};
}
