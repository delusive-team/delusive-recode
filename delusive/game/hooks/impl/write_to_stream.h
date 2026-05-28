#pragma once
#include "../../features/prediction/prediction.h"
#include "../../../configs/configs.h"
#include "../hooks.h"
#include "../../features/weapons/weapons.h"

namespace projectile_shoot {
	static void write_to_stream(void* _this, void* stream) {
		if (core::g_unloading)
			return g_hooks.call(write_to_stream)(_this, stream);

		if (!memory::is_valid(_this) || !memory::is_valid(stream))
			return g_hooks.call(write_to_stream)(_this, stream);

		if (!memory::is_valid(sdk::local_player))
			return g_hooks.call(write_to_stream)(_this, stream);

		features::c_weapons::get().run(sdk::local_player);

		return g_hooks.call(write_to_stream)(_this, stream);
	}
}