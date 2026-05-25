#pragma once
#include "../../sdk/sdk.h"
#include "../../sdk/base_player.h"
#include "../../sdk/input_state.h"
#include "../hooks.h"
#include "../../../configs/configs.h"
#include "../../features/esp/esp.h"

#include "../../features/misc/misc.h"

namespace base_player
{
    void client_input( sdk::BasePlayer* player, sdk::InputState* state )
    {
        if ( core::g_unloading )
            return g_hooks.call( client_input )( player, state );

        if ( !memory::is_valid( player ) )
            return g_hooks.call( client_input )( player, state );

        sdk::camera = sdk::MainCamera::main_camera();
        if (!memory::is_valid(sdk::camera))
        {
            sdk::clear();
            return g_hooks.call(client_input)(player, state);
        }

        if (!sdk::local_player)
            sdk::local_player = player;

        if (memory::is_valid(sdk::local_player)) {
            auto* model = sdk::local_player->model();
            if (memory::is_valid(model)) {
                sdk::local_position = model->position();
            }
        }

        viewmodel();
        auto_reload();
        fake_admin();
        time_changer();
        anti_fly_hack_kick();
        fast_heal();

        if (var->gui.menu_opened) {
            return;
        }

        g_hooks.call( client_input )( player, state );
    }

    void block_sprint(uintptr_t self, float duration) {
        if (core::g_unloading)
            return g_hooks.call(block_sprint)(self, duration);

        if (!memory::is_valid(self))
            return g_hooks.call(block_sprint)(self, duration);

        if (config::misc::movement::disable_melee_slow_down.value)
            duration = 0.0f;

        g_hooks.call(block_sprint)(self, duration);
    }

    void player_walk_movement_client_input(sdk::PlayerWalkMovement* instance, sdk::InputState* state, sdk::ModelState* modelstate) {
        if (core::g_unloading)
            return g_hooks.call(player_walk_movement_client_input)(instance, state, modelstate);

        if (!memory::is_valid(instance) || !memory::is_valid(state) || !memory::is_valid(modelstate))
            return g_hooks.call(player_walk_movement_client_input)(instance, state, modelstate);

        g_hooks.call(player_walk_movement_client_input)(instance, state, modelstate);

        if (!memory::is_valid(sdk::local_player))
            return;

        auto* local_movement = sdk::local_player->walk_movement();
        if (!memory::is_valid(local_movement) || local_movement != instance)
            return;

        movement_action();

        if (config::misc::exploits::exploits_admin_flag.value) {
            modelstate->remove_flag(enums::e_model_state_flags::in_fly);
        }
    }
}
