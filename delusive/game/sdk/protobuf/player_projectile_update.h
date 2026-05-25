#pragma once
#include "../../il2cpp/il2cpp.h"
#include "../math/vector.h"

namespace protobuf {
	class PlayerProjectileUpdate {
	private:
		static inline uintptr_t projectile_id_ptr = 0;
		static inline uintptr_t cur_position_ptr = 0;
		static inline uintptr_t cur_velocity_ptr = 0;
		static inline uintptr_t travel_time_ptr = 0;

	public:
		static void initialize() {
			il2cpp_field_offset(projectile_id_ptr, _("ProtoBuf"), _("PlayerProjectileUpdate"), _("projectileID"));
			il2cpp_field_offset(cur_position_ptr, _("ProtoBuf"), _("PlayerProjectileUpdate"), _("curPosition"));
			il2cpp_field_offset(cur_velocity_ptr, _("ProtoBuf"), _("PlayerProjectileUpdate"), _("curVelocity"));
			il2cpp_field_offset(travel_time_ptr, _("ProtoBuf"), _("PlayerProjectileUpdate"), _("travelTime"));
		}

		il2cpp_field_set(int, set_projectile_id, projectile_id_ptr);
		il2cpp_field_get(vec3_t, cur_position, cur_position_ptr);
		il2cpp_field_set(vec3_t, set_cur_position, cur_position_ptr);
		il2cpp_field_set(vec3_t, set_cur_velocity, cur_velocity_ptr);
		il2cpp_field_set(float, set_travel_time, travel_time_ptr);
	};
}
