#pragma once
#include "../il2cpp/il2cpp.h"
#include "unity/array.h"
#include "unity/transform.h"

namespace sdk {
	class Model {
	private:
		static inline uintptr_t bone_transforms_ptr = 0;

	public:
		static void initialize() {
			il2cpp_field_offset(bone_transforms_ptr, _(""), _("Model"), _("boneTransforms"));
		}

		il2cpp_field_get(unity::c_array<unity::Transform*>*, bone_transforms, bone_transforms_ptr);
	};
}
