#pragma once
#include "../../sdk/sdk.h"
#include "../../sdk/protobuf/player_projectile_attack.h"
#include "../../sdk/protobuf/player_attack.h"
#include "../../sdk/protobuf/attack.h"
#include "../../../configs/configs.h"
#include "../hooks.h"

namespace player_projectile_attack {
    static void write_to_stream(protobuf::PlayerProjectileAttack* self, uintptr_t stream) {
        if (core::g_unloading || !memory::is_valid(self) || !memory::is_valid(sdk::local_player)) {
            return g_hooks.call(write_to_stream)(self, stream);
        }

        if (config::weapons::exploits::hitbox_override.value) {
            auto player_attack = self->player_attack();
            if (memory::is_valid(player_attack)) {
                auto attack = player_attack->attack();
                if (memory::is_valid(attack)) {
                    switch (config::weapons::exploits::hitbox_override_mode.value) {
                    case 0:
                        attack->set_hit_bone(698017942);
                        attack->set_hit_part_id(2173623152);
                        break;
                    case 1: 
                        attack->set_hit_bone(2811218643);
                        attack->set_hit_part_id(1750816991);
                        break;
                    case 2: 
                        switch (rand() % 9) {
                        case 0: attack->set_hit_bone(698017942); attack->set_hit_part_id(2173623152); break;
                        case 1: attack->set_hit_bone(2811218643); attack->set_hit_part_id(1750816991); break;
                        case 2: attack->set_hit_bone(2331610670); attack->set_hit_part_id(1750816991); break;
                        case 3: attack->set_hit_bone(920055401); attack->set_hit_part_id(1750816991); break;
                        case 4: attack->set_hit_bone(3771021956); attack->set_hit_part_id(1750816991); break;
                        case 5: attack->set_hit_bone(736328754); attack->set_hit_part_id(1750816991); break;
                        case 6: attack->set_hit_bone(3901657145); attack->set_hit_part_id(1750816991); break;
                        case 7: attack->set_hit_bone(3892428003); attack->set_hit_part_id(1750816991); break;
                        case 8: attack->set_hit_bone(827230707); attack->set_hit_part_id(1750816991); break;
                        }
                        break;
                    }

                    attack->set_hit_position_local({ -.1f, -.1f, 0.f });
                    attack->set_hit_normal_local({ 0.f, -1.f, 0.f });
                }
            }
        }

        return g_hooks.call(write_to_stream)(self, stream);
    }
}
