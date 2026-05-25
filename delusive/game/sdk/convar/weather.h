#pragma once
#include "../../il2cpp/il2cpp.h"

namespace convar {
	class Weather {
	private:
		static inline uintptr_t m_set_fog = 0;
		static inline uintptr_t m_set_wind = 0;
		static inline uintptr_t m_set_rain = 0;
		static inline uintptr_t m_set_rainbow = 0;
		static inline uintptr_t m_set_thunder = 0;
		static inline uintptr_t m_set_cloud_opacity = 0;
		static inline uintptr_t m_set_atmosphere_contrast = 0;

	public:
		static void initialize() {
			m_set_fog = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("ConVar"), _("Weather")), _("set_fog"), 1));
			m_set_wind = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("ConVar"), _("Weather")), _("set_wind"), 1));
			m_set_rain = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("ConVar"), _("Weather")), _("set_rain"), 1));
			m_set_rainbow = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("ConVar"), _("Weather")), _("set_rainbow"), 1));
			m_set_thunder = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("ConVar"), _("Weather")), _("set_thunder"), 1));
			m_set_cloud_opacity = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("ConVar"), _("Weather")), _("set_cloud_opacity"), 1));
			m_set_atmosphere_contrast = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("ConVar"), _("Weather")), _("set_atmosphere_contrast"), 1));
		}

		static void set_fog(float value) { safe.call<void>(m_set_fog, value); }
		static void set_wind(float value) { safe.call<void>(m_set_wind, value); }
		static void set_rain(float value) { safe.call<void>(m_set_rain, value); }
		static void set_rainbow(float value) { safe.call<void>(m_set_rainbow, value); }
		static void set_thunder(float value) { safe.call<void>(m_set_thunder, value); }
		static void set_cloud_opacity(float value) { safe.call<void>(m_set_cloud_opacity, value); }
		static void set_atmosphere_contrast(float value) { safe.call<void>(m_set_atmosphere_contrast, value); }
	};
}