#pragma once
#include "../../sdk/sdk.h"
#include "../../../configs/configs.h"

namespace aimbot {
	namespace ragebot {
		inline void run() {
			if (!sdk::local_player || !memory::is_valid(sdk::local_player))
				return;
		}
	}
}