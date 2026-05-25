#pragma once
#include "../../il2cpp/il2cpp.h"
#include "../unity/list.h"
#include "projectile.h"

namespace protobuf {
	class ProjectileShoot {
	private:
		static inline uintptr_t projectiles_ptr = 0;

	public:
		static void initialize() {
			il2cpp_field_offset(projectiles_ptr, _("ProtoBuf"), _("ProjectileShoot"), _("projectiles"));
		}

		il2cpp_field_get(unity::c_list<Projectile*>*, projectiles, projectiles_ptr);
	};
}
