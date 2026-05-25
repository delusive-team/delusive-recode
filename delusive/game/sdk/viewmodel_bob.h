#pragma once
#include "../il2cpp/il2cpp.h"
#include "unity/component.h"

namespace sdk {
	class ViewmodelBob : public unity::Component {
	private:
		static inline uintptr_t bob_speed_walk_ptr = 0;
		static inline uintptr_t bob_speed_run_ptr = 0;
		static inline uintptr_t bob_amount_walk_ptr = 0;
		static inline uintptr_t bob_amount_run_ptr = 0;

	public:
		static void initialize() {
			il2cpp_field_offset(bob_speed_walk_ptr, _(""), _("ViewmodelBob"), _("bobSpeedWalk"));
			il2cpp_field_offset(bob_speed_run_ptr, _(""), _("ViewmodelBob"), _("bobSpeedRun"));
			il2cpp_field_offset(bob_amount_walk_ptr, _(""), _("ViewmodelBob"), _("bobAmountWalk"));
			il2cpp_field_offset(bob_amount_run_ptr, _(""), _("ViewmodelBob"), _("bobAmountRun"));
		}

		il2cpp_field_get(float, bob_speed_walk, bob_speed_walk_ptr);
		il2cpp_field_set(float, set_bob_speed_walk, bob_speed_walk_ptr);

		il2cpp_field_get(float, bob_speed_run, bob_speed_run_ptr);
		il2cpp_field_set(float, set_bob_speed_run, bob_speed_run_ptr);

		il2cpp_field_get(float, bob_amount_walk, bob_amount_walk_ptr);
		il2cpp_field_set(float, set_bob_amount_walk, bob_amount_walk_ptr);

		il2cpp_field_get(float, bob_amount_run, bob_amount_run_ptr);
		il2cpp_field_set(float, set_bob_amount_run, bob_amount_run_ptr);
	};
}
