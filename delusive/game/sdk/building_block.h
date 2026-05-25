#pragma once
#include "../il2cpp/il2cpp.h"

namespace sdk {
	class BuildingBlock {
	private:
		static inline uintptr_t upgrade_to_grade_ptr = 0;
		static inline uintptr_t can_afford_upgrade_ptr = 0;
		static inline uintptr_t can_change_to_grade_ptr = 0;

	public:
		static void initialize() {
			auto klass = il2cpp::get_class(_(""), _("BuildingBlock"));
			upgrade_to_grade_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("UpgradeToGrade"), -1));
			can_afford_upgrade_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("CanAffordUpgrade"), -1));
			can_change_to_grade_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("CanChangeToGrade"), -1));
		}

		inline void upgrade(int grade, unsigned int skin) {
			safe.call<void>(upgrade_to_grade_ptr, this, grade, skin, local_player);
		}

		inline bool can_afford(int grade, unsigned int skin) {
			return safe.call<bool>(can_afford_upgrade_ptr, this, grade, skin, local_player);
		}

		inline bool can_upgrade(int grade, unsigned int skin) {
			return safe.call<bool>(can_change_to_grade_ptr, this, grade, skin, local_player);
		}
	};
}
