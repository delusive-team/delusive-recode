#pragma once
#include "../il2cpp/il2cpp.h"

namespace sdk {
	class RecoilProperties {
	private:
		static inline uintptr_t recoil_yaw_min_ptr = 0;
		static inline uintptr_t recoil_yaw_max_ptr = 0;
		static inline uintptr_t recoil_pitch_min_ptr = 0;
		static inline uintptr_t recoil_pitch_max_ptr = 0;

	public:
		static void initialize() {
			il2cpp_field_offset(recoil_yaw_min_ptr, _(""), _("RecoilProperties"), _("recoilYawMin"));
			il2cpp_field_offset(recoil_yaw_max_ptr, _(""), _("RecoilProperties"), _("recoilYawMax"));
			il2cpp_field_offset(recoil_pitch_min_ptr, _(""), _("RecoilProperties"), _("recoilPitchMin"));
			il2cpp_field_offset(recoil_pitch_max_ptr, _(""), _("RecoilProperties"), _("recoilPitchMax"));
		}

		il2cpp_field_get(float, recoil_yaw_min, recoil_yaw_min_ptr);
		il2cpp_field_get(float, recoil_yaw_max, recoil_yaw_max_ptr);
		il2cpp_field_get(float, recoil_pitch_min, recoil_pitch_min_ptr);
		il2cpp_field_get(float, recoil_pitch_max, recoil_pitch_max_ptr);

		il2cpp_field_set(float, set_recoil_yaw_min, recoil_yaw_min_ptr);
		il2cpp_field_set(float, set_recoil_yaw_max, recoil_yaw_max_ptr);
		il2cpp_field_set(float, set_recoil_pitch_min, recoil_pitch_min_ptr);
		il2cpp_field_set(float, set_recoil_pitch_max, recoil_pitch_max_ptr);
	};
}
