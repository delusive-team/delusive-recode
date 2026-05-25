#pragma once
#include "../../sdk/sdk.h"
#include "../../sdk/tod_sky.h"
#include "../../features/visuals/visuals.h"
#include "../hooks.h"

namespace tod_sky {
    static void late_update(sdk::TOD_Sky* self) {
        if (core::g_unloading)
            return g_hooks.call(late_update)(self);

        if (!memory::is_valid(self))
            return g_hooks.call(late_update)(self);

        g_hooks.call(late_update)(self);

        sdk::TOD_CycleParameters* cycle = self->cycle();
        if (memory::is_valid(cycle)) {
            float raw_hour = cycle->hour();
            sdk::server_hour = static_cast<int>(raw_hour);
            sdk::server_minute = static_cast<int>((raw_hour - static_cast<float>(sdk::server_hour)) * 60.0f);
        }

        visuals::tod_sky_late_update(self);
    }
}
