#pragma once
#include "../../sdk/sdk.h"
#include "../hooks.h"

namespace client {
    inline void on_disconnected(uintptr_t self, uintptr_t reason) {
        if (memory::is_valid(sdk::local_player)) {
            LOG("Client disconnected - clearing SDK data");

            sdk::local_player = nullptr;
            sdk::camera = nullptr;
            sdk::local_position = vec3_t{};
            sdk::server_hour = 0;
            sdk::server_minute = 0;

            sdk::info.visible_states.clear();
            sdk::info.player_states.clear();
            sdk::info.bone_positions.clear();

            LOG("SDK data cleared successfully");
        }

        return g_hooks.call(on_disconnected)(self, reason);
    }
}
