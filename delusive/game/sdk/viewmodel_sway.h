#pragma once
#include "../il2cpp/il2cpp.h"
#include "unity/component.h"

namespace sdk {
	class ViewmodelSway : public unity::Component {
	private:
		static inline uintptr_t positional_sway_speed_ptr = 0;
		static inline uintptr_t positional_sway_amount_ptr = 0;
		static inline uintptr_t rotation_sway_speed_ptr = 0;
		static inline uintptr_t rotation_sway_amount_ptr = 0;

	public:
		static void initialize() {
			il2cpp_field_offset(positional_sway_speed_ptr, _(""), _("ViewmodelSway"), _("positionalSwaySpeed"));
			il2cpp_field_offset(positional_sway_amount_ptr, _(""), _("ViewmodelSway"), _("positionalSwayAmount"));
			il2cpp_field_offset(rotation_sway_speed_ptr, _(""), _("ViewmodelSway"), _("rotationSwaySpeed"));
			il2cpp_field_offset(rotation_sway_amount_ptr, _(""), _("ViewmodelSway"), _("rotationSwayAmount"));
		}

		il2cpp_field_get(float, positional_sway_speed, positional_sway_speed_ptr);
		il2cpp_field_set(float, set_positional_sway_speed, positional_sway_speed_ptr);

		il2cpp_field_get(float, positional_sway_amount, positional_sway_amount_ptr);
		il2cpp_field_set(float, set_positional_sway_amount, positional_sway_amount_ptr);

		il2cpp_field_get(float, rotation_sway_speed, rotation_sway_speed_ptr);
		il2cpp_field_set(float, set_rotation_sway_speed, rotation_sway_speed_ptr);

		il2cpp_field_get(float, rotation_sway_amount, rotation_sway_amount_ptr);
		il2cpp_field_set(float, set_rotation_sway_amount, rotation_sway_amount_ptr);
	};
}
