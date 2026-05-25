#pragma once
#include "math/vector.h"

namespace unity {
	class Transform;
	class Component;
}

namespace sdk {
	class BaseEntity;

	struct trace_info_t {
		bool valid; // 0x0
		float distance; // 0x4
		BaseEntity* entity; // 0x8
		vec3_t point; // 0x10
		vec3_t normal; // 0x1C
		unity::Transform* bone; // 0x28
		uintptr_t material; // 0x30
		unsigned int partID; // 0x38
		unity::Component* collider; // 0x40
	};
}
