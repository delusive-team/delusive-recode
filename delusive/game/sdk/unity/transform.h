#pragma once
#include "../../il2cpp/il2cpp.h"
#include "component.h"
#include "../math/vector.h"
#include "../math/matrix.h"

namespace unity {
	struct unity_vec3_t {
		float x;
		float y;
		float z;
	};

	class Transform : public Component {
	private:
		static inline uintptr_t get_local_scale_ptr = 0;
		static inline uintptr_t set_local_scale_ptr = 0;
		static inline uintptr_t get_euler_angles_ptr = 0;
		static inline uintptr_t inverse_transform_point_ptr = 0;
		static inline uintptr_t inverse_transform_direction_ptr = 0;

		static inline uintptr_t set_position_injected_ptr = 0;
		static inline uintptr_t get_position_injected_ptr = 0;
		static inline uintptr_t set_rotation_injected_ptr = 0;
		static inline uintptr_t get_rotation_injected_ptr = 0;
		static inline uintptr_t transform_point_injected_ptr = 0;
		static inline uintptr_t get_local_position_injected_ptr = 0;
		static inline uintptr_t get_local_to_world_matrix_injected_ptr = 0;

	public:
		static void initialize() {
			get_local_scale_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Transform")), _("get_localScale"), -1));
			set_local_scale_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Transform")), _("set_localScale"), -1));
			get_euler_angles_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Transform")), _("get_eulerAngles"), -1));
			inverse_transform_point_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Transform")), _("InverseTransformPoint"), 1));
			inverse_transform_direction_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Transform")), _("InverseTransformDirection"), 1));

			set_position_injected_ptr = il2cpp::resolve_icall(_("UnityEngine.Transform::set_position_Injected(UnityEngine.Vector3&)"));
			get_position_injected_ptr = il2cpp::resolve_icall(_("UnityEngine.Transform::get_position_Injected(UnityEngine.Vector3&)"));
			set_rotation_injected_ptr = il2cpp::resolve_icall(_("UnityEngine.Transform::set_rotation_Injected(UnityEngine.Quaternion&)"));
			get_rotation_injected_ptr = il2cpp::resolve_icall(_("UnityEngine.Transform::get_rotation_Injected(UnityEngine.Quaternion&)"));
			transform_point_injected_ptr = il2cpp::resolve_icall(_("UnityEngine.Transform::TransformPoint_Injected"));
			get_local_position_injected_ptr = il2cpp::resolve_icall(_("UnityEngine.Transform::get_localPosition_Injected(UnityEngine.Vector3&)"));
			get_local_to_world_matrix_injected_ptr = il2cpp::resolve_icall(_("UnityEngine.Transform::get_localToWorldMatrix_Injected"));
		}

		inline quat_t rotation() {
			if (!memory::is_valid(this)) return { };
			quat_t result;
			safe.call<void>(get_rotation_injected_ptr, this, &result);
			return result;
		}

		inline mat4x4_t local_to_world_matrix() {
			if (!memory::is_valid(this)) return { };
			mat4x4_t result;
			safe.call<void>(get_local_to_world_matrix_injected_ptr, this, &result);
			return result;
		}

		inline vec3_t right() {
			return rotation() * vec3_t{1.f, 0.f, 0.f};
		}

		inline vec3_t up() {
			return rotation() * vec3_t{0.f, 1.f, 0.f};
		}

		inline vec3_t forward() {
			return rotation() * vec3_t{0.f, 0.f, 1.f};
		}

		inline vec3_t position() {
			if (!memory::is_valid(this)) return { };
			vec3_t result;
			safe.call<void>(get_position_injected_ptr, this, &result);
			return result;
		}

		inline vec3_t local_position() {
			if (!memory::is_valid(this)) return { };
			vec3_t result;
			safe.call<void>(get_local_position_injected_ptr, this, &result);
			return result;
		}

		inline void set_rotation(quat_t pos) {
			safe.call<void>(set_rotation_injected_ptr, this, pos);
		}

		inline void set_position(vec3_t pos) {
			safe.call<void>(set_position_injected_ptr, this, pos);
		}

		inline vec3_t local_scale() {
			return safe.call<vec3_t>(get_local_scale_ptr, this);
		}

		inline vec3_t euler_angles() {
			return safe.call<vec3_t>(get_euler_angles_ptr, this);
		}

		inline void set_local_scale(vec3_t scale) {
			safe.call<void>(set_local_scale_ptr, this, scale);
		}

		inline vec3_t inverse_transform_point(vec3_t pt) {
			if (!memory::is_valid(this)) return { };
			return safe.call<vec3_t>(inverse_transform_point_ptr, this, pt);
		}

		inline vec3_t inverse_transform_direction(vec3_t pt) {
			if (!memory::is_valid(this)) return { };
			return safe.call<vec3_t>(inverse_transform_direction_ptr, this, pt);
		}

		inline vec2_t transform_point(vec2_t pt) {
			unity_vec3_t result{ };
			safe.call<void>(transform_point_injected_ptr, this, pt, &result);
			return { result.x, result.y };
		}
	};
}
