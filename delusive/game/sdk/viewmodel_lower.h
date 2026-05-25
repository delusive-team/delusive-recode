#pragma once
#include "../il2cpp/il2cpp.h"
#include "unity/component.h"

namespace sdk {
	class ViewmodelLower : public unity::Component {
	private:
		static inline uintptr_t lower_on_sprint_ptr = 0;
		static inline uintptr_t should_lower_ptr = 0;
		static inline uintptr_t lower_scale_lower_ptr = 0;

	public:
		static void initialize() {
			il2cpp_field_offset(lower_on_sprint_ptr, _(""), _("ViewmodelLower"), _("lowerOnSprint"));
			il2cpp_field_offset(should_lower_ptr, _(""), _("ViewmodelLower"), _("shouldLower"));
			il2cpp_field_offset(lower_scale_lower_ptr, _(""), _("ViewmodelLower"), _("lowerScale"));
		}

		il2cpp_field_get(bool, lower_on_sprint, lower_on_sprint_ptr);
		il2cpp_field_set(bool, set_lower_on_sprint, lower_on_sprint_ptr);
		il2cpp_field_set(float, set_lower_scale, lower_scale_lower_ptr);

		il2cpp_field_get(bool, should_lower, should_lower_ptr);
		il2cpp_field_set(bool, set_should_lower, should_lower_ptr);
	};
}
