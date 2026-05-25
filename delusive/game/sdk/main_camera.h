#pragma once
#include "../il2cpp/il2cpp.h"

namespace unity {
	class Camera;
}

namespace sdk {
	class MainCamera {
	private:
		static inline uintptr_t main_camera_ptr = 0;

	public:
		static void initialize() {
			il2cpp_static_field(main_camera_ptr, _(""), _("MainCamera"), _("mainCamera"));
		}

		static unity::Camera* main_camera() {
			return il2cpp::get_static_field<unity::Camera*>(main_camera_ptr);
		}
	};
}
