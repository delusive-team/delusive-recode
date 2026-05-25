#pragma once
#include "../../il2cpp/il2cpp.h"
#include "collider.h"
#include "../math/vector.h"

namespace unity {
	class SphereCollider : public Collider {
	private:
		static inline uintptr_t set_center_ptr = 0;
		static inline uintptr_t get_center_ptr = 0;
		static inline uintptr_t set_radius_ptr = 0;
		static inline uintptr_t get_radius_ptr = 0;

	public:
		static void initialize() {
			auto klass = il2cpp::get_class(_("UnityEngine"), _("SphereCollider"));
			set_center_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("set_center"), 1));
			get_center_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("get_center"), -1));
			set_radius_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("set_radius"), 1));
			get_radius_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("get_radius"), -1));
		}

		inline void set_center(vec3_t center) { safe.call<void>(set_center_ptr, this, center); }
		inline vec3_t center() { return safe.call<vec3_t>(get_center_ptr, this); }

		inline void set_radius(float radius) { safe.call<void>(set_radius_ptr, this, radius); }
		inline float radius() { return safe.call<float>(get_radius_ptr, this); }
	};
}
