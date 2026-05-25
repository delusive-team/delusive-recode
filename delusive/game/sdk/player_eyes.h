#pragma once
#include "../il2cpp/il2cpp.h"
#include "math/vector.h"

namespace sdk {
	class PlayerEyes {
	private:
		static inline uintptr_t view_offset_ptr = 0;
		static inline uintptr_t body_rotation_ptr = 0;

		static inline uintptr_t body_right_ptr = 0;
		static inline uintptr_t get_center_ptr = 0;
		static inline uintptr_t body_forward_ptr = 0;
		static inline uintptr_t head_forward_ptr = 0;
		static inline uintptr_t set_rotation_ptr = 0;
		static inline uintptr_t get_rotation_ptr = 0;
		static inline uintptr_t get_position_ptr = 0;
		static inline uintptr_t movement_right_ptr = 0;
		static inline uintptr_t set_body_rotation_ptr = 0;

	public:
		static void initialize() {
			auto klass = il2cpp::get_class(_(""), _("PlayerEyes"));
			
			il2cpp_field_offset(view_offset_ptr, _(""), _("PlayerEyes"), _("viewOffset"));
			il2cpp_field_offset(body_rotation_ptr, _(""), _("PlayerEyes"), _("<bodyRotation>k__BackingField"));

			body_right_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("BodyRight"), -1));
			get_center_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("get_center"), -1));
			body_forward_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("BodyForward"), -1));
			head_forward_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("HeadForward"), -1));
			set_rotation_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("set_rotation"), -1));
			get_rotation_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("get_rotation"), -1));
			get_position_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("get_position"), -1));
			movement_right_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("MovementRight"), -1));
			set_body_rotation_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("set_bodyRotation"), 1));
		}

		il2cpp_field_get(quat_t, body_rotation, body_rotation_ptr);
		il2cpp_field_get(vec3_t, view_offset, view_offset_ptr);
		il2cpp_field_set(vec3_t, set_view_offset, view_offset_ptr);

		inline vec3_t center() { return safe.call<vec3_t>(get_center_ptr, this); }
		inline vec3_t position() { return safe.call<vec3_t>(get_position_ptr, this); }
		inline quat_t rotation() { return safe.call<quat_t>(get_rotation_ptr, this); }
		inline void set_rotation(quat_t rot) { safe.call<void>(set_rotation_ptr, this, rot); }
		inline void set_body_rotation(quat_t rotation) { safe.call<void>(set_body_rotation_ptr, this, rotation); }
		inline vec3_t head_forward() { return safe.call<vec3_t>(head_forward_ptr, this); }
		inline vec3_t body_forward() { return safe.call<vec3_t>(body_forward_ptr, this); }
		inline vec3_t body_right() { return safe.call<vec3_t>(body_right_ptr, this); }
		inline vec3_t movement_right() { return safe.call<vec3_t>(movement_right_ptr, this); }
	};
}
