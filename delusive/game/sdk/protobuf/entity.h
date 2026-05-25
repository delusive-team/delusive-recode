#pragma once
#include "../../il2cpp/il2cpp.h"
#include "environment.h"

namespace protobuf {
	class Entity {
	private:
		static inline uintptr_t environment_ptr = 0;

	public:
		static void initialize() {
			il2cpp_field_offset(environment_ptr, _("ProtoBuf"), _("Entity"), _("environment"));
		}

		il2cpp_field_get(Environment*, environment, environment_ptr);
	};
}
