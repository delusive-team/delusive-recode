#pragma once
#include "../../sdk/sdk.h"
#include "../../sdk/steam_platform.h"
#include "../../sdk/unity/input.h"
#include "../hooks.h"
#include "../../../configs/configs.h"

namespace steam_platform {
    static void steam_platform_update(sdk::SteamPlatform* instance) {
        g_hooks.call(steam_platform_update)(instance);

        ImGui::GetIO().MouseWheel += unity::Input::mouse_scroll_delta().y / 4.0f;

        if (config::misc::exploits::exploits_name_spoofer.value) {
            static bool name_changed = false;
            bool is_on_server = memory::is_valid(sdk::local_player);

            if (is_on_server && !name_changed) {
                unsigned int random_id = (rand() % 100) + 1;
                auto* new_name = sdk::RandomUsernames::get(random_id);
                if (memory::is_valid(new_name)) {
                    instance->set_username(new_name);
                    name_changed = true;
                }
            }

            if (!is_on_server && name_changed) {
                name_changed = false;
            }
        }
    }
}
