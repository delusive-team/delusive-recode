#pragma once
#include "../../il2cpp/il2cpp.h"
#include "attack.h"

namespace protobuf {
	class PlayerAttack {
	private:
		static inline uintptr_t attack_ptr = 0;
		static inline uintptr_t projectile_id_ptr = 0;

	public:
		static void initialize() {
			il2cpp_field_offset(attack_ptr, _("ProtoBuf"), _("PlayerAttack"), _("attack"));
			il2cpp_field_offset(projectile_id_ptr, _("ProtoBuf"), _("PlayerAttack"), _("projectileID"));
		}

		il2cpp_field_get(Attack*, attack, attack_ptr);
		il2cpp_field_set(Attack*, set_attack, attack_ptr);
		il2cpp_field_set(int, set_projectile_id, projectile_id_ptr);
	};
}
