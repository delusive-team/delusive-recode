#pragma once
#include "../../sdk/sdk.h"
#include "../hooks.h"
#include "../../features/misc/misc.h"
#include "../../../framework/settings/variables.h"

namespace client
{
    void late_update( uintptr_t self )
    {
        if ( core::g_unloading ) {
            g_is_on_server = false;
            g_is_reloading = false;
            g_reload_progress = 0.0f;
            convar::Graphics::set_fov(90.f); 
            if (memory::is_valid(sdk::camera)) {
                float default_aspect = 1.777f;
                float w = (float)unity::Screen::width();
                float h = (float)unity::Screen::height();
                if (h > 0.f) {
                    default_aspect = w / h;
                }
                sdk::camera->set_aspect(default_aspect);
            }
            reset_camera_mode(sdk::local_player);
            return g_hooks.call( late_update )( self );
        }

        if ( !memory::is_valid( self ) )
            return g_hooks.call( late_update )( self );

        g_hooks.call( late_update )( self );

        if ( var->gui.menu_opened ) {
        }

        g_is_on_server = memory::is_valid(sdk::local_player);
        bool is_reloading = false;
        float progress = 0.0f;
        if (g_is_on_server) {
            auto* held_entity = sdk::local_player->held_entity();
            if (memory::is_valid(held_entity) && held_entity->is_class(_("BaseProjectile"))) {
                float finish_time = held_entity->next_reload_time();
                float current_time = unity::Time::time();

                if (finish_time > current_time) {
                    float duration = held_entity->reload_time();
                    if (duration > 0.f) {
                        is_reloading = true;
                        progress = 1.0f - ((finish_time - current_time) / duration);
                        progress = std::clamp(progress, 0.0f, 1.0f);
                    }
                }
            }
        }
        g_is_reloading = is_reloading;
        g_reload_progress = progress;

        viewmodel_changer();
        fov_changer();
        aspect_ratio_changer();
        camera_mode();

        bool is_bullet_cam_enabled = config::misc::view::misc_camera_to_bullet.value;
        int bullet_cam_key = config::misc::view::misc_camera_to_bullet_key.value;
        int bullet_cam_key_mode = config::misc::view::misc_camera_to_bullet_key_mode.value;
        
        bool is_bullet_cam_key_down = false;
        if (bullet_cam_key != 0) {
            if (bullet_cam_key_mode == 0) { // Hold
                is_bullet_cam_key_down = (GetAsyncKeyState(bullet_cam_key) & 0x8000) != 0;
            }
            else if (bullet_cam_key_mode == 1) { // Toggle
                static bool toggle_state = false;
                static bool was_pressed = false;
                bool is_pressed = (GetAsyncKeyState(bullet_cam_key) & 0x8000) != 0;
                if (is_pressed && !was_pressed) {
                    toggle_state = !toggle_state;
                }
                was_pressed = is_pressed;
                is_bullet_cam_key_down = toggle_state;
            }
            else if (bullet_cam_key_mode == 2) { // Always on
                is_bullet_cam_key_down = true;
            }
        }

        static sdk::Projectile* tracked_bullet = nullptr;
        static float last_click_time = 0.f;

        if (is_bullet_cam_enabled && is_bullet_cam_key_down) {
            if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
                last_click_time = unity::Time::time();
            }

            if (tracked_bullet && (!memory::is_valid(tracked_bullet) || tracked_bullet->traveled_time() >= 8.f)) {
                tracked_bullet = nullptr;
            }

            if (!tracked_bullet && (unity::Time::time() - last_click_time < 0.5f)) {
                uintptr_t proj_type = il2cpp::type_object("", "Projectile");
                if (proj_type) {
                    unity::c_array<sdk::Projectile*>* projs = unity::Object::find_objects_of_type<sdk::Projectile*>(proj_type);
                    if (projs && projs->count() > 0) {
                        float max_time = -1.f;
                        for (int i = 0; i < projs->count(); ++i) {
                            sdk::Projectile* proj = projs->get(i);
                            if (memory::is_valid(proj)) {
                                sdk::BasePlayer* owner = proj->owner();
                                if (memory::is_valid(owner) && owner->is_local_player()) {
                                    float t = proj->traveled_time();
                                    if (t > max_time) {
                                        max_time = t;
                                        tracked_bullet = proj;
                                    }
                                }
                            }
                        }
                    }
                }
            }

            if (tracked_bullet && memory::is_valid(tracked_bullet)) {
                auto* transform = tracked_bullet->transform();
                if (memory::is_valid(transform)) {
                    vec3_t bullet_pos = transform->position();
                    vec3_t bullet_vel = tracked_bullet->current_velocity();
                    
                    vec3_t dir = bullet_vel.normalized();
                    bullet_camera_pos = bullet_pos - (dir * 2.5f);
                    
                    if (bullet_vel.magnitude() > 0.1f) {
                        bullet_camera_rot = unity::Quaternion::look_rotation(dir);
                    }
                    bullet_camera_active = true;
                } else {
                    bullet_camera_active = false;
                }
            } else {
                bullet_camera_active = false;
            }
        } else {
            bullet_camera_active = false;
            tracked_bullet = nullptr;
        }

        if (bullet_camera_active && memory::is_valid(sdk::camera)) {
            auto* cam_trans = sdk::camera->transform();
            if (memory::is_valid(cam_trans)) {
                cam_trans->set_position(bullet_camera_pos);
                cam_trans->set_rotation(bullet_camera_rot);
            }
        }
    }
}
