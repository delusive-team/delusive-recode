#pragma once
#include "../il2cpp/il2cpp.h"
#include "tod_day_parameters.h"
#include "tod_sun_parameters.h"
#include "tod_night_parameters.h"
#include "tod_ambient_parameters.h"

namespace sdk {
	class TOD_StarsParameters {
	private:
		static inline uintptr_t size_ptr = 0;
		static inline uintptr_t brightness_ptr = 0;
		static inline uintptr_t position_ptr = 0;

	public:
		static void initialize() {
			il2cpp_field_offset(size_ptr, _(""), _("TOD_StarParameters"), _("Size"));
			il2cpp_field_offset(brightness_ptr, _(""), _("TOD_StarParameters"), _("Brightness"));
			il2cpp_field_offset(position_ptr, _(""), _("TOD_StarParameters"), _("Position"));
		}

		il2cpp_field_get(float, size, size_ptr);
		il2cpp_field_set(float, set_size, size_ptr);
		il2cpp_field_get(float, brightness, brightness_ptr);
		il2cpp_field_set(float, set_brightness, brightness_ptr);
		il2cpp_field_set(int, set_position, position_ptr);
	};

	class TOD_MoonParameters {
	private:
		static inline uintptr_t mesh_size_ptr = 0;
		static inline uintptr_t mesh_brightness_ptr = 0;
		static inline uintptr_t mesh_contrast_ptr = 0;
		static inline uintptr_t halo_size_ptr = 0;
		static inline uintptr_t halo_brightness_ptr = 0;
		static inline uintptr_t position_ptr = 0;

	public:
		static void initialize() {
			il2cpp_field_offset(mesh_size_ptr, _(""), _("TOD_MoonParameters"), _("MeshSize"));
			il2cpp_field_offset(mesh_brightness_ptr, _(""), _("TOD_MoonParameters"), _("MeshBrightness"));
			il2cpp_field_offset(mesh_contrast_ptr, _(""), _("TOD_MoonParameters"), _("MeshContrast"));
			il2cpp_field_offset(halo_size_ptr, _(""), _("TOD_MoonParameters"), _("HaloSize"));
			il2cpp_field_offset(halo_brightness_ptr, _(""), _("TOD_MoonParameters"), _("HaloBrightness"));
			il2cpp_field_offset(position_ptr, _(""), _("TOD_MoonParameters"), _("Position"));
		}

		il2cpp_field_set(float, set_mesh_size, mesh_size_ptr);
		il2cpp_field_set(float, set_mesh_brightness, mesh_brightness_ptr);
		il2cpp_field_set(float, set_mesh_contrast, mesh_contrast_ptr);
		il2cpp_field_set(float, set_halo_size, halo_size_ptr);
		il2cpp_field_set(float, set_halo_brightness, halo_brightness_ptr);
		il2cpp_field_set(int, set_position, position_ptr);
	};

	class TOD_Sky {
	private:
		static inline uintptr_t day_ptr = 0;
		static inline uintptr_t sun_ptr = 0;
		static inline uintptr_t night_ptr = 0;
		static inline uintptr_t ambient_ptr = 0;
		static inline uintptr_t cycle_ptr = 0;
		static inline uintptr_t stars_ptr = 0;
		static inline uintptr_t moon_ptr = 0;
		static inline uintptr_t clouds_ptr = 0;
		static inline uintptr_t world_ptr = 0;
		static inline uintptr_t atmosphere_ptr = 0;
		static inline uintptr_t get_instance_ptr = 0;

	public:
		static void initialize() {
			il2cpp_field_offset(day_ptr, _(""), _("TOD_Sky"), _("Day"));
			il2cpp_field_offset(sun_ptr, _(""), _("TOD_Sky"), _("Sun"));
			il2cpp_field_offset(night_ptr, _(""), _("TOD_Sky"), _("Night"));
			il2cpp_field_offset(ambient_ptr, _(""), _("TOD_Sky"), _("Ambient"));
			il2cpp_field_offset(cycle_ptr, _(""), _("TOD_Sky"), _("Cycle"));
			il2cpp_field_offset(stars_ptr, _(""), _("TOD_Sky"), _("Stars"));
			il2cpp_field_offset(moon_ptr, _(""), _("TOD_Sky"), _("Moon"));
			il2cpp_field_offset(clouds_ptr, _(""), _("TOD_Sky"), _("Clouds"));
			il2cpp_field_offset(world_ptr, _(""), _("TOD_Sky"), _("World"));
			il2cpp_field_offset(atmosphere_ptr, _(""), _("TOD_Sky"), _("Atmosphere"));

			get_instance_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_(""), _("TOD_Sky")), _("get_Instance"), -1));
		}

		il2cpp_field_get(TOD_DayParameters*, day, day_ptr);
		il2cpp_field_get(TOD_SunParameters*, sun, sun_ptr);
		il2cpp_field_get(TOD_NightParameters*, night, night_ptr);
		il2cpp_field_get(TOD_AmbientParameters*, ambient, ambient_ptr);
		il2cpp_field_get(TOD_CycleParameters*, cycle, cycle_ptr);
		il2cpp_field_get(TOD_StarsParameters*, stars, stars_ptr);
		il2cpp_field_get(TOD_MoonParameters*, moon, moon_ptr);
		il2cpp_field_get(TOD_CloudParameters*, clouds, clouds_ptr);
		il2cpp_field_get(TOD_WorldParameters*, world, world_ptr);
		il2cpp_field_get(TOD_AtmosphereParameters*, atmosphere, atmosphere_ptr);

		static TOD_Sky* instance() {
			return safe.call<TOD_Sky*>(get_instance_ptr);
		}
	};
}
