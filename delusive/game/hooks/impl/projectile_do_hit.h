#pragma once
#include "../../sdk/sdk.h"
#include "../hooks.h"
#include "../../../configs/configs.h"

namespace hooks {
    inline bool projectile_do_hit(sdk::Projectile* self, sdk::HitTest* test, vec3_t point, vec3_t normal) {
        typedef bool(*do_hit_fn)(sdk::Projectile*, sdk::HitTest*, vec3_t, vec3_t);
        auto original = g_hooks.call<do_hit_fn>(projectile_do_hit);

        if (core::g_unloading || !original)
            return original ? original(self, test, point, normal) : true;

        if (!memory::is_valid(self) || !memory::is_valid(test))
            return original(self, test, point, normal);

        if (config::weapons::exploits::weapon_penetration.value) {
            auto* entity = test->hit_entity();

            // If no entity hit (wall/terrain/static world) — penetrate through
            if (!memory::is_valid(entity)) {
                return false;
            }

            const char* class_name = entity->class_name();
            if (class_name) {
                // Only let the shot connect with actual players/NPCs
                bool is_player_or_npc = strstr(class_name, _("BasePlayer")) ||
                                        strstr(class_name, _("NPCPlayer")) ||
                                        strstr(class_name, _("HumanNPC")) ||
                                        strstr(class_name, _("ScientistNPC")) ||
                                        strstr(class_name, _("BradleyAPC")) ||
                                        strstr(class_name, _("BaseAnimalNPC"));
                if (!is_player_or_npc) {
                    return false;
                }
            } else {
                // No class name means unknown entity — penetrate
                return false;
            }
        }

        return original(self, test, point, normal);
    }
}
