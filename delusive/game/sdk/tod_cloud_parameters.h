#pragma once
#include "../il2cpp/il2cpp.h"

namespace sdk {
	class TOD_CloudParameters {
	private:
		static inline uintptr_t size_ptr = 0;
		static inline uintptr_t opacity_ptr = 0;
		static inline uintptr_t coverage_ptr = 0;
		static inline uintptr_t sharpness_ptr = 0;
		static inline uintptr_t coloring_ptr = 0;
		static inline uintptr_t attenuation_ptr = 0;
		static inline uintptr_t saturation_ptr = 0;
		static inline uintptr_t scattering_ptr = 0;
		static inline uintptr_t brightness_ptr = 0;

	public:
		static void initialize() {
			il2cpp_field_offset(size_ptr, _(""), _("TOD_CloudParameters"), _("Size"));
			il2cpp_field_offset(opacity_ptr, _(""), _("TOD_CloudParameters"), _("Opacity"));
			il2cpp_field_offset(coverage_ptr, _(""), _("TOD_CloudParameters"), _("Coverage"));
			il2cpp_field_offset(sharpness_ptr, _(""), _("TOD_CloudParameters"), _("Sharpness"));
			il2cpp_field_offset(coloring_ptr, _(""), _("TOD_CloudParameters"), _("Coloring"));
			il2cpp_field_offset(attenuation_ptr, _(""), _("TOD_CloudParameters"), _("Attenuation"));
			il2cpp_field_offset(saturation_ptr, _(""), _("TOD_CloudParameters"), _("Saturation"));
			il2cpp_field_offset(scattering_ptr, _(""), _("TOD_CloudParameters"), _("Scattering"));
			il2cpp_field_offset(brightness_ptr, _(""), _("TOD_CloudParameters"), _("Brightness"));
		}

		il2cpp_field_set(float, set_size, size_ptr);
		il2cpp_field_set(float, set_opacity, opacity_ptr);
		il2cpp_field_set(float, set_coverage, coverage_ptr);
		il2cpp_field_set(float, set_sharpness, sharpness_ptr);
		il2cpp_field_set(float, set_coloring, coloring_ptr);
		il2cpp_field_set(float, set_attenuation, attenuation_ptr);
		il2cpp_field_set(float, set_saturation, saturation_ptr);
		il2cpp_field_set(float, set_scattering, scattering_ptr);
		il2cpp_field_set(float, set_brightness, brightness_ptr);
	};
}
