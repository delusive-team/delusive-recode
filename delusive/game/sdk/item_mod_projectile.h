#pragma once
#include "../il2cpp/il2cpp.h"
#include "game_object_ref.h"

namespace sdk {
	class ItemModProjectile {
	private:
		static inline uintptr_t projectile_object_ptr = 0;
		static inline uintptr_t projectile_spread_ptr = 0;
		static inline uintptr_t projectile_velocity_ptr = 0;
		static inline uintptr_t projectile_velocity_spread_ptr = 0;

	public:
		static void initialize() {
			il2cpp_field_offset(projectile_object_ptr, _(""), _("ItemModProjectile"), _("projectileObject"));
			il2cpp_field_offset(projectile_spread_ptr, _(""), _("ItemModProjectile"), _("projectileSpread"));
			il2cpp_field_offset(projectile_velocity_ptr, _(""), _("ItemModProjectile"), _("projectileVelocity"));
			il2cpp_field_offset(projectile_velocity_spread_ptr, _(""), _("ItemModProjectile"), _("projectileVelocitySpread"));
		}

		il2cpp_field_get(GameObjectRef*, projectile_object, projectile_object_ptr);
		il2cpp_field_get(float, projectile_spread, projectile_spread_ptr);
		il2cpp_field_get(float, projectile_velocity, projectile_velocity_ptr);
		il2cpp_field_get(float, projectile_velocity_spread, projectile_velocity_spread_ptr);

		il2cpp_field_set(float, set_projectile_spread, projectile_spread_ptr);
		il2cpp_field_set(float, set_projectile_velocity, projectile_velocity_ptr);
		il2cpp_field_set(float, set_projectile_velocity_spread, projectile_velocity_spread_ptr);
	};
}
