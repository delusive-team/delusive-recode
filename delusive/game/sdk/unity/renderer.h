#pragma once
#include "../../il2cpp/il2cpp.h"
#include "component.h"
#include "material.h"

namespace unity {
	class Renderer : public Component {
	private:
		static inline uintptr_t get_enabled_ptr = 0;
		static inline uintptr_t set_material_ptr = 0;
		static inline uintptr_t get_material_ptr = 0;

	public:
		static void initialize() {
			get_enabled_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Renderer")), _("get_enabled"), -1));
			set_material_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Renderer")), _("set_material"), 1));
			get_material_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Renderer")), _("get_material"), -1));
		}

		inline Material* material() {
			return safe.call<Material*>(get_material_ptr, this);
		}

		inline void set_material(Material* material) {
			safe.call<void>(set_material_ptr, this, material);
		}

		inline bool enabled() {
			return safe.call<bool>(get_enabled_ptr, this);
		}
	};
}
