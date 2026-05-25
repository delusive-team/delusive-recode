#pragma once
#include "../il2cpp/il2cpp.h"

namespace sdk {
	class TOD_DayParameters {
	private:
		static inline uintptr_t light_intensity_ptr = 0;
		static inline uintptr_t ambient_multiplier_ptr = 0;
		static inline uintptr_t reflection_multiplier_ptr = 0;
		static inline uintptr_t shadow_strength_ptr = 0;

	public:
		static void initialize() {
			il2cpp_field_offset(light_intensity_ptr, _(""), _("TOD_DayParameters"), _("LightIntensity"));
			il2cpp_field_offset(ambient_multiplier_ptr, _(""), _("TOD_DayParameters"), _("AmbientMultiplier"));
			il2cpp_field_offset(reflection_multiplier_ptr, _(""), _("TOD_DayParameters"), _("ReflectionMultiplier"));
			il2cpp_field_offset(shadow_strength_ptr, _(""), _("TOD_DayParameters"), _("ShadowStrength"));
		}

		il2cpp_field_set(float, set_light_intensity, light_intensity_ptr);
		il2cpp_field_set(float, set_ambient_multiplier, ambient_multiplier_ptr);
		il2cpp_field_set(float, set_reflection_multiplier, reflection_multiplier_ptr);
		il2cpp_field_set(float, set_shadow_strength, shadow_strength_ptr);
	};
}
