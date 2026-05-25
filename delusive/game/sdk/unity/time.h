#pragma once
#include "../../il2cpp/il2cpp.h"

namespace unity {
	class Time {
	private:
		static inline uintptr_t get_time_ptr = 0;
		static inline uintptr_t get_time_scale_ptr = 0;
		static inline uintptr_t set_time_scale_ptr = 0;
		static inline uintptr_t get_fixed_time_ptr = 0;
		static inline uintptr_t get_delta_time_ptr = 0;
		static inline uintptr_t get_fixed_delta_time_ptr = 0;
		static inline uintptr_t get_smooth_delta_time_ptr = 0;
		static inline uintptr_t get_realtime_since_startup_ptr = 0;

	public:
		static void initialize() {
			get_time_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Time")), _("get_time"), -1));
			get_time_scale_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Time")), _("get_timeScale"), -1));
			set_time_scale_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Time")), _("set_timeScale"), 1));
			get_fixed_time_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Time")), _("get_fixedTime"), -1));
			get_delta_time_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Time")), _("get_deltaTime"), -1));
			get_fixed_delta_time_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Time")), _("get_fixedDeltaTime"), -1));
			get_smooth_delta_time_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Time")), _("get_smoothDeltaTime"), -1));
			get_realtime_since_startup_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Time")), _("get_realtimeSinceStartup"), -1));
		}

		static float time() {
			return safe.call<float>(get_time_ptr);
		}

		static float timescale() {
			return safe.call<float>(get_time_scale_ptr);
		}

		static void set_timescale(float scale) {
			safe.call<void>(set_time_scale_ptr, scale);
		}

		static float delta_time() {
			return safe.call<float>(get_delta_time_ptr);
		}

		static float fixed_delta_time() {
			return safe.call<float>(get_fixed_delta_time_ptr);
		}

		static float smooth_delta_time() {
			return safe.call<float>(get_smooth_delta_time_ptr);
		}

		static float fixed_time() {
			return safe.call<float>(get_fixed_time_ptr);
		}

		static float realtime_since_startup() {
			return safe.call<float>(get_realtime_since_startup_ptr);
		}
	};
}
