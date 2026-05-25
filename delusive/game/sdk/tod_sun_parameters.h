#pragma once
#include "../il2cpp/il2cpp.h"

namespace sdk {
	class TOD_SunParameters {
	private:
		static inline uintptr_t mesh_size_ptr = 0;
		static inline uintptr_t mesh_contrast_ptr = 0;
		static inline uintptr_t mesh_brightness_ptr = 0;

	public:
		static void initialize() {
			il2cpp_field_offset(mesh_size_ptr, _(""), _("TOD_SunParameters"), _("MeshSize"));
			il2cpp_field_offset(mesh_contrast_ptr, _(""), _("TOD_SunParameters"), _("MeshContrast"));
			il2cpp_field_offset(mesh_brightness_ptr, _(""), _("TOD_SunParameters"), _("MeshBrightness"));
		}

		il2cpp_field_set(float, set_mesh_size, mesh_size_ptr);
		il2cpp_field_set(float, set_mesh_contrast, mesh_contrast_ptr);
		il2cpp_field_set(float, set_mesh_brightness, mesh_brightness_ptr);
	};
}
