#pragma once
#include <algorithm>
#include <cmath>
#include "../../sdk/sdk.h"
#include "../../sdk/tod_sky.h"
#include "../../sdk/main_camera.h"
#include "../../sdk/unity/camera.h"
#include "../../sdk/convar/weather.h"
#include "../../../configs/configs.h"

namespace core {
    extern volatile bool g_unloading;
}

namespace visuals {
    struct sky_backup_t {
        float stars_size;
        float stars_brightness;
        ImVec4 night_sky_color;
        ImVec4 day_sky_color;
        ImVec4 night_cloud_color;
        ImVec4 day_cloud_color;
        ImVec4 night_fog_color;
        ImVec4 day_fog_color;
        ImVec4 night_world_color;
        ImVec4 day_world_color;
        bool initialized = false;
    };

    inline sky_backup_t sky_backup;

    inline ImVec4* get_color_ptr(uintptr_t base, uint32_t offset) {
        if (!memory::is_valid(base)) return nullptr;

        uintptr_t p1 = mem::read<uintptr_t>(base + offset);
        if (!memory::is_valid(p1)) return nullptr;

        uintptr_t inst = mem::read<uintptr_t>(p1 + 0x10);
        if (!memory::is_valid(inst)) return nullptr;

        return reinterpret_cast<ImVec4*>(inst);
    }

    inline ImVec4 get_color(uintptr_t base, uint32_t offset) {
        if (!memory::is_valid(base))
            return ImVec4(0, 0, 0, 0);

        ImVec4* color_ptr = get_color_ptr(base, offset);
        if (color_ptr && memory::is_valid(reinterpret_cast<uintptr_t>(color_ptr)))
            return *color_ptr;

        return ImVec4(0, 0, 0, 0);
    }

    inline void set_color(uintptr_t base, uint32_t offset, ImVec4 color) {
        if (!memory::is_valid(base))
            return;

        if (color.x != color.x || color.y != color.y ||
            color.z != color.z || color.w != color.w)
            return;

        ImVec4* color_ptr = get_color_ptr(base, offset);
        if (color_ptr && memory::is_valid(reinterpret_cast<uintptr_t>(color_ptr)))
            *color_ptr = color;
    }

    inline void backup_sky_settings(sdk::TOD_Sky* tod_sky) {
        if (sky_backup.initialized || !memory::is_valid(tod_sky))
            return;

        auto* stars = tod_sky->stars();
        if (memory::is_valid(stars)) {
            sky_backup.stars_size = stars->size();
            sky_backup.stars_brightness = stars->brightness();
        }

        auto* night = tod_sky->night();
        auto* day = tod_sky->day();

        uintptr_t n_base = reinterpret_cast<uintptr_t>(night);
        uintptr_t d_base = reinterpret_cast<uintptr_t>(day);

        if (memory::is_valid(n_base) && memory::is_valid(d_base)) {
            sky_backup.night_sky_color = get_color(n_base, 0x28);
            sky_backup.day_sky_color = get_color(d_base, 0x28);

            sky_backup.night_cloud_color = get_color(n_base, 0x30);
            sky_backup.day_cloud_color = get_color(d_base, 0x30);

            sky_backup.night_fog_color = get_color(n_base, 0x38);
            sky_backup.day_fog_color = get_color(d_base, 0x38);

            sky_backup.night_world_color = get_color(n_base, 0x40);
            sky_backup.day_world_color = get_color(d_base, 0x40);

            sky_backup.initialized = true;
        }
    }

    inline void apply_custom_color(uintptr_t n_base, uintptr_t d_base, uint32_t offset,
                                   bool active, bool rainbow, ImVec4 target,
                                   ImVec4 bk_night, ImVec4 bk_day) {
        if (active) {
            if (rainbow) {
                float r, g, b;
                ImGui::ColorConvertHSVtoRGB(fmodf(ImGui::GetTime() * 0.3f, 1.0f), 1.0f, 1.0f, r, g, b);
                target = ImVec4(r, g, b, target.w);
            }
            set_color(n_base, offset, target);
            set_color(d_base, offset, target);
        } else {
            set_color(n_base, offset, bk_night);
            set_color(d_base, offset, bk_day);
        }
    }

    inline void update_weather() {
        constexpr float def = -1.f;

        if (core::g_unloading) {
            convar::Weather::set_fog(def);
            convar::Weather::set_rain(def);
            convar::Weather::set_thunder(def);
            convar::Weather::set_wind(def);
            convar::Weather::set_rainbow(def);
            convar::Weather::set_cloud_opacity(def);
            convar::Weather::set_atmosphere_contrast(def);
            return;
        }

        using namespace config::visuals;

        convar::Weather::set_cloud_opacity(weather::removals::visuals_weather_no_clouds.value ? 0.f : def);
        convar::Weather::set_thunder(weather::removals::visuals_weather_no_thunder.value ? 0.f : def);
        convar::Weather::set_wind(weather::removals::visuals_weather_no_wind.value ? 0.f : def);
        convar::Weather::set_rainbow(weather::removals::visuals_weather_rainbows.value ? 1.f : def);

        float fog = def;
        if (weather::removals::visuals_weather_no_fog.value)
            fog = 0.f;
        else if (weather::modify_fog.value)
            fog = std::clamp(weather::modify_fog_value.value, 0.f, 1.f);
        convar::Weather::set_fog(fog);

        float rain = def;
        if (weather::removals::visuals_weather_no_rain.value)
            rain = 0.f;
        else if (weather::modify_snow.value)
            rain = std::clamp(weather::modify_snow_value.value, 0.f, 1.f);
        else if (weather::modify_rain.value)
            rain = std::clamp(weather::modify_rain_value.value, 0.f, 1.f);
        convar::Weather::set_rain(rain);

        convar::Weather::set_atmosphere_contrast(
            worlds::atmosphere_contrast.value ?
            std::clamp((float)worlds::atmosphere_contrast_value.value, 0.f, 5.f) : def
        );
    }

    inline void custom_sun(sdk::TOD_SunParameters* params) {
        if (!params || !memory::is_valid(params))
            return;

        if (config::visuals::worlds::sun_size_override.value)
            params->set_mesh_size(config::visuals::worlds::sun_size.value);

        if (config::visuals::worlds::sun_brightness_override.value)
            params->set_mesh_brightness(config::visuals::worlds::sun_brightness.value);
    }

    inline void custom_moon(sdk::TOD_MoonParameters* params) {
        if (!params || !memory::is_valid(params))
            return;

        if (config::visuals::worlds::moon_size_override.value) {
            auto v = config::visuals::worlds::moon_size.value;
            if (v == v && v >= 0.0f)
                params->set_mesh_size(v);
        }

        if (config::visuals::worlds::moon_brightness_override.value) {
            auto v = config::visuals::worlds::moon_brightness.value;
            if (v == v)
                params->set_mesh_brightness(v);
        }

        if (config::visuals::worlds::moon_phase_override.value) {
            auto v = config::visuals::worlds::moon_phase.value;
            if (v == v)
                params->set_mesh_contrast(v);
        }

        if (config::visuals::worlds::moon_halo.value) {
            params->set_halo_size(config::visuals::worlds::moon_halo_size.value);
            params->set_halo_brightness(config::visuals::worlds::moon_halo_brightness.value);
        }
        params->set_position(config::visuals::worlds::moon_position.value);
    }

    inline void custom_clouds(sdk::TOD_CloudParameters* params) {
        if (!params || !memory::is_valid(params))
            return;

        if (config::visuals::worlds::custom_clouds.value) {
            params->set_size(config::visuals::worlds::cloud_size.value);
            params->set_opacity(config::visuals::worlds::cloud_opacity.value);
            params->set_coverage(config::visuals::worlds::cloud_coverage.value);
            params->set_sharpness(config::visuals::worlds::cloud_sharpness.value);
            params->set_coloring(config::visuals::worlds::cloud_coloring.value);
            params->set_attenuation(config::visuals::worlds::cloud_attenuation.value);
            params->set_saturation(config::visuals::worlds::cloud_saturation.value);
            params->set_scattering(config::visuals::worlds::cloud_scattering.value);
            params->set_brightness(config::visuals::worlds::cloud_brightness.value);
        }
    }

    inline void layer_changer() {
        auto* camera = sdk::MainCamera::main_camera();
        if (!memory::is_valid(camera))
            return;

        int mask = camera->culling_mask();
        int old_mask = mask;

        int key = config::visuals::layers::layers_bind.value;
        int mode = config::visuals::layers::layers_mode.value;
        bool layers_active = false;

        if (key != 0) {
            if (mode == 0) {
                layers_active = (GetAsyncKeyState(key) & 0x8000) != 0;
            }
            else if (mode == 1) {
                static bool toggle_state = false;
                static bool was_pressed = false;
                bool is_pressed = (GetAsyncKeyState(key) & 0x8000) != 0;
                if (is_pressed && !was_pressed) {
                    toggle_state = !toggle_state;
                }
                was_pressed = is_pressed;
                layers_active = toggle_state;
            }
            else if (mode == 2) {
                layers_active = true;
            }
        } else if (config::visuals::layers::layers_enable.value) {
            layers_active = true;
        }

        bool active = layers_active && !core::g_unloading;

        auto process_layer = [&](bool feature_enabled, int layer_index) {
            if (feature_enabled && active)
                mask &= ~(1 << layer_index);
            else
                mask |= (1 << layer_index);
        };

        process_layer(config::visuals::layers::layers_remove_water.value, 4);
        process_layer(config::visuals::layers::layers_remove_ragdolls.value, 9);
        process_layer(config::visuals::layers::layers_remove_buildings.value, 21);
        process_layer(config::visuals::layers::layers_remove_terrain.value, 23);
        process_layer(config::visuals::layers::layers_remove_cargoship.value, 27);
        process_layer(config::visuals::layers::layers_remove_trees.value, 30);

        if (mask != old_mask)
            camera->set_culling_mask(mask);
    }

    inline void tod_sky_late_update(sdk::TOD_Sky* tod_sky) {
        if (!memory::is_valid(tod_sky))
            return;

        auto* night = tod_sky->night();
        auto* day = tod_sky->day();
        auto* stars = tod_sky->stars();
        auto* sun = tod_sky->sun();
        auto* cycle = tod_sky->cycle();

        if (!memory::is_valid(night) || !memory::is_valid(day) || !memory::is_valid(stars))
            return;

        backup_sky_settings(tod_sky);
        update_weather();

        using namespace config::visuals;
        bool unl = core::g_unloading;

        float shadow_str = (weather::removals::visuals_weather_no_sun.value && !unl) ? 0.f : 1.f;
        day->set_shadow_strength(shadow_str);
        night->set_shadow_strength(shadow_str);

        if (memory::is_valid(sun)) {
            float sun_mesh = (weather::removals::visuals_weather_no_sun.value && !unl) ? 0.f : 1.f;
            sun->set_mesh_brightness(sun_mesh);
            sun->set_mesh_contrast(sun_mesh);
            sun->set_mesh_size(sun_mesh);
        }

        if (worlds::visuals_sky_bright_night.value && !unl) {
            float current_hour = cycle->hour();
            if (current_hour >= 18.0f || current_hour <= 6.0f) {
                night->set_light_intensity(0.f);
                night->set_shadow_strength(0.f);
                night->set_ambient_multiplier((float)worlds::visuals_sky_bright_night_ambient_multiplier.value);
                night->set_reflection_multiplier((float)worlds::visuals_sky_bright_night_reflection_multiplier.value);
            } else {
                day->set_ambient_multiplier(1.f);
                day->set_shadow_strength(1.f);
            }
        } else if (worlds::visuals_sky_bright_cave.value && !unl) {
            day->set_ambient_multiplier(1.f);
            day->set_reflection_multiplier(1.f);
            day->set_shadow_strength(0.f);
        } else {
            day->set_ambient_multiplier(1.f);
            day->set_shadow_strength(1.f);
            night->set_ambient_multiplier(1.f);
            night->set_reflection_multiplier(1.f);
            night->set_shadow_strength(1.f);
            night->set_light_intensity(1.f);
        }

        if (worlds::night_stars.value && !unl) {
            stars->set_size((float)worlds::night_stars_size.value);
            stars->set_brightness((float)worlds::night_stars_brightness.value);
            stars->set_position(worlds::night_stars_position.value);
        } else {
            stars->set_size(sky_backup.stars_size);
            stars->set_brightness(sky_backup.stars_brightness);
            stars->set_position(0);
        }

        uintptr_t n_base = reinterpret_cast<uintptr_t>(night);
        uintptr_t d_base = reinterpret_cast<uintptr_t>(day);

        apply_custom_color(n_base, d_base, 0x28,
            worlds::sky_color_changer.value && !unl,
            worlds::sky_color_changer_rainbow.value,
            worlds::sky_color_changer_color.value,
            sky_backup.night_sky_color, sky_backup.day_sky_color);

        apply_custom_color(n_base, d_base, 0x30,
            worlds::cloud_color_changer.value && !unl,
            worlds::cloud_color_changer_rainbow.value,
            worlds::cloud_color_changer_color.value,
            sky_backup.night_cloud_color, sky_backup.day_cloud_color);

        apply_custom_color(n_base, d_base, 0x38,
            worlds::fog_color_changer.value && !unl,
            worlds::fog_color_changer_rainbow.value,
            worlds::fog_color_changer_color.value,
            sky_backup.night_fog_color, sky_backup.day_fog_color);

        apply_custom_color(n_base, d_base, 0x40,
            worlds::world_color_changer.value && !unl,
            worlds::world_color_changer_rainbow.value,
            worlds::world_color_changer_color.value,
            sky_backup.night_world_color, sky_backup.day_world_color);

        custom_sun(sun);
        custom_moon(tod_sky->moon());
        custom_clouds(tod_sky->clouds());

        auto* world = tod_sky->world();
        if (memory::is_valid(world)) {
            if (worlds::custom_latitude.value && !unl) {
                world->set_latitude(worlds::custom_latitude_value.value);
            } else {
                world->set_latitude(0.0f);
            }

            if (worlds::custom_longitude.value && !unl) {
                world->set_longitude(worlds::custom_longitude_value.value);
            } else {
                world->set_longitude(0.0f);
            }
        }

        auto* atmosphere = tod_sky->atmosphere();
        if (memory::is_valid(atmosphere)) {
            if (worlds::rayleigh_multiplier.value && !unl) {
                atmosphere->set_rayleigh_multiplier(worlds::rayleigh_multiplier_value.value);
            } else {
                atmosphere->set_rayleigh_multiplier(1.0f);
            }
        }

        if (worlds::sky_light_intensity.value && !unl) {
            day->set_light_intensity(worlds::sky_day_light_intensity_value.value);
            night->set_light_intensity(worlds::sky_night_light_intensity_value.value);
        }

        if (worlds::sky_shadow_strength.value && !unl) {
            day->set_shadow_strength(worlds::sky_day_shadow_strength_value.value);
            night->set_shadow_strength(worlds::sky_night_shadow_strength_value.value);
        }

        layer_changer();
    }

    inline void restore_all() {
        core::g_unloading = true;
        update_weather();

        auto* tod_sky = sdk::TOD_Sky::instance();
        if (memory::is_valid(tod_sky))
            tod_sky_late_update(tod_sky);

        auto* camera = sdk::MainCamera::main_camera();
        if (memory::is_valid(camera)) {
            int mask = camera->culling_mask();
            mask |= (1 << 4);
            mask |= (1 << 21);
            mask |= (1 << 23);
            mask |= (1 << 27);
            mask |= (1 << 30);
            camera->set_culling_mask(mask);
        }
    }
}
