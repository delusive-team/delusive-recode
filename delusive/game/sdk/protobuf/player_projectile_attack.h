#pragma once
#include "../../il2cpp/il2cpp.h"
#include "../math/vector.h"
#include "player_attack.h"

namespace protobuf {
	class PlayerProjectileAttack {
	private:
		static inline uintptr_t hit_velocity_ptr = 0;
		static inline uintptr_t hit_distance_ptr = 0;
		static inline uintptr_t player_attack_ptr = 0;

	public:
		static void initialize() {
			il2cpp_field_offset(hit_velocity_ptr, _("ProtoBuf"), _("PlayerProjectileAttack"), _("hitVelocity"));
			il2cpp_field_offset(hit_distance_ptr, _("ProtoBuf"), _("PlayerProjectileAttack"), _("hitDistance"));
			il2cpp_field_offset(player_attack_ptr, _("ProtoBuf"), _("PlayerProjectileAttack"), _("playerAttack"));
		}
	};
}
