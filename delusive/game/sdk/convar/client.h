#pragma once
#include "../../il2cpp/il2cpp.h"

namespace convar {
	class Client {
	private:
		static inline uintptr_t cam_lerp_ptr = 0;
		static inline uintptr_t cam_speed_ptr = 0;
		static inline uintptr_t cam_dist_ptr = 0;

	public:
		static void initialize() {
			il2cpp_static_field(cam_lerp_ptr, _("ConVar"), _("Client"), _("camlerp"));
			il2cpp_static_field(cam_speed_ptr, _("ConVar"), _("Client"), _("camspeed"));
			il2cpp_static_field(cam_dist_ptr, _("ConVar"), _("Client"), _("camdist"));
		}

		static float cam_lerp() { return il2cpp::get_static_field<float>(cam_lerp_ptr); }
		static float cam_speed() { return il2cpp::get_static_field<float>(cam_speed_ptr); }
		static float cam_dist() { return il2cpp::get_static_field<float>(cam_dist_ptr); }

		static void set_cam_lerp(float value) { il2cpp::set_static_field<float>(cam_lerp_ptr, value); }
		static void set_cam_speed(float value) { il2cpp::set_static_field<float>(cam_speed_ptr, value); }
		static void set_cam_dist(float value) { il2cpp::set_static_field<float>(cam_dist_ptr, value); }
	};
}
