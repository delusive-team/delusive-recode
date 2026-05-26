#pragma once
#include "../../sdk/sdk.h"
#include "../../sdk/base_melee.h"
#include "../../sdk/item.h"
#include "../hooks.h"
#include "../../../configs/configs.h"

namespace base_melee {
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
