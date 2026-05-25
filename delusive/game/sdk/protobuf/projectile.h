#pragma once
#include "../../il2cpp/il2cpp.h"
#include "../math/vector.h"

namespace protobuf {
	class Projectile {
	private:
		static inline uintptr_t start_pos_ptr = 0;
		static inline uintptr_t start_vel_ptr = 0;

	public:
		static void initialize() {
			il2cpp_field_offset_nested(start_pos_ptr, _("ProtoBuf"), _("ProjectileShoot"), _("Projectile"), _("startPos"));
			il2cpp_field_offset_nested(start_vel_ptr, _("ProtoBuf"), _("ProjectileShoot"), _("Projectile"), _("startVel"));
		}

		il2cpp_field_get(vec3_t, start_pos, start_pos_ptr);
		il2cpp_field_set(vec3_t, set_start_pos, start_pos_ptr);

		il2cpp_field_get(vec3_t, start_vel, start_vel_ptr);
		il2cpp_field_set(vec3_t, set_start_vel, start_vel_ptr);
	};
}
