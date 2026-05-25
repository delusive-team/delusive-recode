#pragma once
#include "../../il2cpp/il2cpp.h"
#include "../math/vector.h"

namespace unity {
	class Quaternion {
	private:
		static inline uintptr_t euler_ptr = 0;
		static inline uintptr_t internal_make_positive_ptr = 0;

		static inline uintptr_t slerp_injected_ptr = 0;
		static inline uintptr_t look_rotation_injected_ptr = 0;
		static inline uintptr_t internal_to_euler_rad_injected_ptr = 0;

	public:
		static void initialize() {
			euler_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Quaternion")), _("Euler"), 3));
			internal_make_positive_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Quaternion")), _("Internal_MakePositive"), 1));

			slerp_injected_ptr = il2cpp::resolve_icall(_("UnityEngine.Quaternion::Slerp_Injected"));
			look_rotation_injected_ptr = il2cpp::resolve_icall(_("UnityEngine.Quaternion::LookRotation_Injected(UnityEngine.Vector3&,UnityEngine.Vector3&,UnityEngine.Quaternion&)"));
			internal_to_euler_rad_injected_ptr = il2cpp::resolve_icall(_("UnityEngine.Quaternion::Internal_ToEulerRad_Injected(UnityEngine.Quaternion&,UnityEngine.Vector3&)"));
		}

		static vec3_t make_positive(const vec3_t& euler) {
			return safe.call<vec3_t>(internal_make_positive_ptr, euler);
		}

		static vec3_t to_euler_rad(const quat_t& rotation) {
			vec3_t result;
			safe.call<void>(internal_to_euler_rad_injected_ptr, rotation, &result);
			return result;
		}

		static quat_t euler(vec3_t euler_vec) {
			return safe.call<quat_t>(euler_ptr, euler_vec.x, euler_vec.y, euler_vec.z);
		}

		static quat_t look_rotation(const vec3_t& forward) {
			auto upwards = vec3_t{0.f, 1.f, 0.f};
			quat_t result;
			safe.call<void>(look_rotation_injected_ptr, forward, upwards, &result);
			return result;
		}

		static quat_t slerp(quat_t a, quat_t b, float t) {
			quat_t result;
			safe.call<void>(slerp_injected_ptr, a, b, t, &result);
			return result;
		}

		static vec3_t euler_angles(const quat_t& rotation) {
			return make_positive(to_euler_rad(rotation) * 57.29578f);
		}
	};
}
