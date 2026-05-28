#pragma once
#include "../../sdk/sdk.h"
#include "../../sdk/base_player.h"
#include "../../sdk/input_state.h"
#include "../hooks.h"
#include "../../../configs/configs.h"
#include "../../features/esp/esp.h"

#include "../../features/misc/misc.h"
#include "../../features/visuals/visuals.h"
#include "../../features/aimbot/legitbot.h"
#include "../../sdk/convar/admin.h"
#include "../../features/weapons/weapons.h"

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

        std::vector<sdk::BasePlayer*> players_copy;
        {
            std::lock_guard<std::mutex> lock(sdk::info.mtx);
            players_copy = sdk::info.players;
        }

        for (auto&& player : players_copy) {
            if (player->is_local_player() || player->lifestate())
                continue;

            player->set_bone_positions();

            const auto& head_position = player->bone_position(enums::e_bone::head);

            unity::raycast_hit_t hit_info;
            if (sdk::GamePhysics::trace(unity::ray_t(head_position, head_position + vec3_t(0.f, 500.f, 0.f)), 0.f, hit_info, 500.f, 2097152, enums::e_query_trigger_interaction::ignore, nullptr)) {
                player->set_player_states(enums::e_player_state::inside);
            }
            else {
                player->set_player_states(enums::e_player_state::outside);
            }

            if (config::esp::players_flag_visible) {
                const auto& transform = sdk::camera->transform();
                if (memory::is_valid(transform))
                    player->set_visible_state(sdk::GamePhysics::is_visible(transform->position(), player->bone_position(enums::e_bone::head)));
            }
        }

        viewmodel();
        auto_reload();
        fake_admin();
        time_changer();
        anti_fly_hack_kick();
        fast_heal();
        attack_actions();
        exploits_actions();
        spinbot(state, player);
        aimbot::legitbot::run();

        auto* tod_sky = sdk::TOD_Sky::instance();
        if (memory::is_valid(tod_sky)) {
            sdk::TOD_CycleParameters* cycle = tod_sky->cycle();
            if (memory::is_valid(cycle)) {
                float raw_hour = cycle->hour();
                sdk::server_hour = static_cast<int>(raw_hour);
                sdk::server_minute = static_cast<int>((raw_hour - static_cast<float>(sdk::server_hour)) * 60.0f);
            }
            visuals::tod_sky_late_update(tod_sky);
        }

        if (var->gui.menu_opened) {
            return;
        }

        if (config::visuals::weather::removals::visuals_weather_no_underwater.value) {
            convar::Admin::set_underwater_effect(true);
        } else {
            convar::Admin::set_underwater_effect(false);
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
