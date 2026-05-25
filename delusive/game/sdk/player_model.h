#pragma once
#include "../il2cpp/il2cpp.h"
#include "math/vector.h"
#include "skinned_multi_mesh.h"
#include "model.h"

namespace sdk {
	class PlayerModel : public Model {
	private:
		static inline uintptr_t velocity_ptr = 0;
		static inline uintptr_t position_ptr = 0;
		static inline uintptr_t multi_mesh_ptr = 0;
		static inline uintptr_t new_velocity_ptr = 0;
		static inline uintptr_t is_local_player_ptr = 0;

		static inline uintptr_t get_is_npc_ptr = 0;
		static inline uintptr_t rebuild_all_ptr = 0;

	public:
		static void initialize() {
			auto klass = il2cpp::get_class(_(""), _("PlayerModel"));
			il2cpp_field_offset(velocity_ptr, _(""), _("PlayerModel"), _("velocity"));
			il2cpp_field_offset(position_ptr, _(""), _("PlayerModel"), _("position"));
			il2cpp_field_offset(multi_mesh_ptr, _(""), _("PlayerModel"), _("_multiMesh"));
			il2cpp_field_offset(new_velocity_ptr, _(""), _("PlayerModel"), _("newVelocity"));
			il2cpp_field_offset(is_local_player_ptr, _(""), _("PlayerModel"), _("isLocalPlayer"));

			get_is_npc_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("get_IsNpc"), -1));
			rebuild_all_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("RebuildAll"), -1));
		}

		il2cpp_field_get(vec3_t, velocity, velocity_ptr);
		il2cpp_field_get(vec3_t, position, position_ptr);
		il2cpp_field_get(SkinnedMultiMesh*, multi_mesh, multi_mesh_ptr);
		il2cpp_field_get(vec3_t, new_velocity, new_velocity_ptr);
		il2cpp_field_get(bool, is_local_player, is_local_player_ptr);

		inline bool is_npc() {
			return safe.call<bool>(get_is_npc_ptr, this);
		}

		static void rebuild_all() {
			safe.call<void>(rebuild_all_ptr);
		}
	};
}
