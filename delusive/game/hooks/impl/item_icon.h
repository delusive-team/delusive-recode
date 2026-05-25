#pragma once
#include "../../sdk/sdk.h"
#include "../../sdk/item_icon.h"
#include "../hooks.h"
#include "../../../configs/configs.h"

namespace item_icon
{
    void try_to_move( sdk::ItemIcon* instance )
    {
        if ( core::g_unloading )
            return g_hooks.call( try_to_move )( instance );

        if ( !memory::is_valid( sdk::local_player ) ) {
            return g_hooks.call( try_to_move )( instance );
        }

        g_hooks.call( try_to_move )( instance );
        
        if (config::exploits_fast_loot && instance->queued_for_looting()) {
            instance->run_timed_action( );
        }
    }
}
