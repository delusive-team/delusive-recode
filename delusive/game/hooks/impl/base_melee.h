#pragma once
#include "../../sdk/sdk.h"
#include "../../sdk/base_melee.h"
#include "../../sdk/item.h"
#include "../../sdk/item_mod_projectile.h"
#include "../../features/prediction/prediction.h"
#include "../../features/aimbot/legitbot.h"
#include "../../features/aimbot/ragebot.h"
#include "../hooks.h"
#include "../../../configs/configs.h"

namespace base_melee {
    static void on_viewmodel_event(sdk::BaseMelee* self, unity::c_string* name) {
        if (core::g_unloading || !config::aimbot::rage_enabled.value)
            return g_hooks.call(on_viewmodel_event)(self, name);

        bool is_throw = false;
        if (memory::is_valid(name) && name->string().length() > 0) {
            is_throw = (name->string() == _("Throw"));
        }

        if (!is_throw)
            return g_hooks.call(on_viewmodel_event)(self, name);

        auto* local = sdk::local_player;
        if (!memory::is_valid(local) || local->lifestate() != 0)
            return g_hooks.call(on_viewmodel_event)(self, name);

        // Use tracked silent target OR find the best target in the scene
        sdk::BasePlayer* target = aimbot::ragebot::g_silent_target;
        if (!memory::is_valid(target)) {
            auto* eyes_tmp = local->eyes();
            if (memory::is_valid(eyes_tmp)) {
                auto res = aimbot::ragebot::find_best_target(eyes_tmp->position(), 180.f);
                target = res.player;
            }
        }
        if (!memory::is_valid(target))
            return g_hooks.call(on_viewmodel_event)(self, name);

        auto* item = local->item();
        if (!memory::is_valid(item))
            return g_hooks.call(on_viewmodel_event)(self, name);

        auto* info = item->info();
        if (!memory::is_valid(info))
            return g_hooks.call(on_viewmodel_event)(self, name);

        auto item_mod_projectile_type = il2cpp::type_object(_(""), _("ItemModProjectile"));
        auto* item_mod = info->component<sdk::ItemModProjectile*>(item_mod_projectile_type);
        if (!memory::is_valid(item_mod))
            return g_hooks.call(on_viewmodel_event)(self, name);

        float original_spread     = item_mod->projectile_spread();
        float original_vel_spread = item_mod->projectile_velocity_spread();
        float original_speed      = item_mod->projectile_velocity();

        float new_speed = (original_speed + original_vel_spread);
        if (config::weapons::instant_bullet.value || config::aimbot::rage_bullet_tp.value) {
            new_speed *= 1.495f;
        }

        vec2_t old_angles  = { 0, 0 };
        bool angles_changed = false;

        auto* eyes = local->eyes();
        if (memory::is_valid(eyes)) {
            vec3_t start_pos  = eyes->position();
            vec3_t target_pos = target->bone_position(enums::e_bone::head);
            vec3_t target_vel = target->get_world_velocity();

            float realtime   = unity::Time::realtime_since_startup();
            float last_tick  = local->last_sent_tick_time();
            float desync     = std::clamp(realtime - last_tick, 0.f, 1.f);
            float dsv        = (std::max)(desync - (0.03125f * 5.f), 0.2f);
            float initial_dist = 0.f;
            if (config::weapons::instant_bullet.value || config::aimbot::rage_bullet_tp.value) {
                initial_dist = dsv * new_speed;
            }

            vec3_t aim_point = prediction::c_prediction::get_aim_direction(start_pos, target_pos, target_vel, new_speed, initial_dist);

            if (!aim_point.empty()) {
                auto* input = local->input();
                if (memory::is_valid(input)) {
                    old_angles = input->body_angles();
                    vec2_t throw_angles = aimbot::legitbot::calc_angle(start_pos, aim_point);

                    // Set both body angles and eyes rotation for accurate server prediction
                    input->set_body_angles(throw_angles);
                    eyes->set_rotation(quat_t::from_euler(throw_angles.x, throw_angles.y, 0.f));
                    angles_changed = true;
                }
            }
        }

        item_mod->set_projectile_velocity_spread(0.f);
        item_mod->set_projectile_spread(0.f);
        item_mod->set_projectile_velocity(new_speed);

        g_hooks.call(on_viewmodel_event)(self, name);

        if (angles_changed) {
            auto* input = local->input();
            if (memory::is_valid(input)) {
                input->set_body_angles(old_angles);
            }
            if (memory::is_valid(eyes)) {
                eyes->set_rotation(quat_t::from_euler(old_angles.x, old_angles.y, 0.f));
            }
        }

        item_mod->set_projectile_velocity_spread(original_vel_spread);
        item_mod->set_projectile_spread(original_spread);
        item_mod->set_projectile_velocity(original_speed);
    }

    static void do_attack(sdk::BaseMelee* instance, sdk::Item* item) {
        if (core::g_unloading)
            return g_hooks.call(do_attack)(instance, item);

        if (!memory::is_valid(instance))
            return g_hooks.call(do_attack)(instance, item);

        const bool extended = config::misc::exploits::exploits_extended_melee.value;

        float original_distance = 0.f;
        float original_radius = 0.f;

        if (extended) {
            original_distance = instance->max_distance();
            original_radius = instance->attack_radius();

            instance->set_max_distance(original_distance * 1.42f);
            instance->set_attack_radius(original_radius * 1.82f);
        }

        g_hooks.call(do_attack)(instance, item);

        if (extended) {
            instance->set_max_distance(original_distance);
            instance->set_attack_radius(original_radius);
        }
    }

    static void process_attack(sdk::BaseMelee* instance, sdk::HitTest* hit) {
        if (core::g_unloading || !memory::is_valid(instance) || !memory::is_valid(hit))
            return g_hooks.call(process_attack)(instance, hit);

        bool farm_ore = config::misc::automatic::ore_assist_farm.value;
        bool farm_tree = config::misc::automatic::tree_assist_farm.value;

        if (!farm_ore && !farm_tree)
            return g_hooks.call(process_attack)(instance, hit);

        auto* hit_entity = hit->hit_entity();
        if (!memory::is_valid(hit_entity))
            return g_hooks.call(process_attack)(instance, hit);

        const char* class_name = hit_entity->class_name();
        if (!class_name)
            return g_hooks.call(process_attack)(instance, hit);

        auto* hit_entity_transform = hit_entity->transform();
        if (!memory::is_valid(hit_entity_transform))
            return g_hooks.call(process_attack)(instance, hit);

        vec3_t hit_entity_pos = hit_entity_transform->position();

        if (farm_ore && strstr(class_name, _("OreResourceEntity"))) {
            auto* hot_spot = sdk::BaseEntity::find_closest<sdk::BaseEntity*>(_("OreHotSpot"), 3.0f);

            if (memory::is_valid(hot_spot)) {
                auto* hot_spot_transform = hot_spot->transform();
                if (memory::is_valid(hot_spot_transform)) {
                    hit->set_hit_transform(hot_spot_transform);

                    vec3_t hot_spot_pos = hot_spot_transform->position();
                    vec3_t local_point = hot_spot_transform->inverse_transform_point(hot_spot_pos);

                    hit->set_hit_point(local_point);
                }
            }
        }
        else if (farm_tree && strstr(class_name, _("TreeEntity"))) {
            auto* marker = sdk::BaseEntity::find_closest<sdk::BaseEntity*>(_("TreeMarker"), 3.0f);

            if (memory::is_valid(marker)) {
                auto* marker_transform = marker->transform();

                if (memory::is_valid(marker_transform)) {
                    vec3_t marker_pos = marker_transform->position();

                    vec3_t local_hit_point = hit_entity_transform->inverse_transform_point(marker_pos);
                    hit->set_hit_point(local_hit_point);

                    if (memory::is_valid(sdk::local_player)) {
                        float dist = sdk::local_player->bone_position(enums::e_bone::head).distance_to(marker_pos);
                        hit->set_hit_distance(dist);
                    }

                    vec3_t dir = vec3_t(hit_entity_pos.x, 0, hit_entity_pos.z) - vec3_t(marker_pos.x, 0, marker_pos.z);
                    hit_entity->set_hit_direction(dir);
                }
            }
        }

        g_hooks.call(process_attack)(instance, hit);
    }
}
