#pragma once
#include "../il2cpp/il2cpp.h"
#include "base_entity.h"

namespace sdk {
	class BaseCombatEntity : public BaseEntity {
	private:
		static inline uintptr_t lifestate_ptr = 0;
		static inline uintptr_t health_ptr = 0;
		static inline uintptr_t max_health_ptr = 0;

		static inline uintptr_t is_dead_ptr = 0;

	public:
		static void initialize() {
			auto klass = il2cpp::get_class(_(""), _("BaseCombatEntity"));

			il2cpp_field_offset(lifestate_ptr, _(""), _("BaseCombatEntity"), _("lifestate"));
			il2cpp_field_offset(health_ptr, _(""), _("BaseCombatEntity"), _("_health"));
			il2cpp_field_offset(max_health_ptr, _(""), _("BaseCombatEntity"), _("_maxHealth"));

			is_dead_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("IsDead"), -1));
		}

		il2cpp_field_get(bool, lifestate, lifestate_ptr);
		il2cpp_field_get(float, health, health_ptr);
		il2cpp_field_get(float, max_health, max_health_ptr);

		inline bool is_dead() {
			return safe.call<bool>(is_dead_ptr, this);
		}

		inline bool is_player() {
			const char* name = this->class_name();
			if (!name || !*name) return false;

			static const char* k_player_names[] = {
				"BasePlayer",
				"NPCPlayerApex",
				"NPCMurderer",
				"NPCPlayer",
				"HumanNPC",
				"Scientist",
				"TunnelDweller",
				"HTNPlayer",
				"HumanNPCNew",
				"ScientistNPCNew",
				"BanditGuard",
				"ScientistNPC",
				"NPCMissionProvider",
				"NPCShopkeeper"
			};

			for (const auto* target : k_player_names) {
				if (strcmp(name, target) == 0)
					return true;
			}

			return false;
		}
	};
}
