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
}
