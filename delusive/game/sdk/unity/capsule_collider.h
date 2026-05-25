#pragma once
#include "../../il2cpp/il2cpp.h"
#include "collider.h"
#include "../math/vector.h"

namespace unity {
	class CapsuleCollider : public Collider {
	private:
		static inline uintptr_t set_radius_ptr = 0;
		static inline uintptr_t get_radius_ptr = 0;
		static inline uintptr_t set_height_ptr = 0;
		static inline uintptr_t get_height_ptr = 0;
		static inline uintptr_t set_direction_ptr = 0;
		static inline uintptr_t get_direction_ptr = 0;
		static inline uintptr_t set_center_ptr = 0;
		static inline uintptr_t get_center_ptr = 0;

	public:
		static void initialize() {
			auto klass = il2cpp::get_class(_("UnityEngine"), _("CapsuleCollider"));
			set_radius_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("set_radius"), 1));
			get_radius_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("get_radius"), -1));
			set_height_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("set_height"), 1));
			get_height_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("get_height"), -1));
			set_direction_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("set_direction"), 1));
			get_direction_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("get_direction"), -1));
			set_center_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("set_center"), 1));
			get_center_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("get_center"), -1));
		}

		inline void set_radius(float radius) { safe.call<void>(set_radius_ptr, this, radius); }
		inline float radius() { return safe.call<float>(get_radius_ptr, this); }

		inline void set_height(float height) { safe.call<void>(set_height_ptr, this, height); }
		inline float height() { return safe.call<float>(get_height_ptr, this); }

		inline void set_direction(int direction) { safe.call<void>(set_direction_ptr, this, direction); }
		inline int direction() { return safe.call<int>(get_direction_ptr, this); }

		inline void set_center(vec3_t center) { safe.call<void>(set_center_ptr, this, center); }
		inline vec3_t center() { return safe.call<vec3_t>(get_center_ptr, this); }
	};
}
