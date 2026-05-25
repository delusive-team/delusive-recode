#pragma once
#include "../../il2cpp/il2cpp.h"
#include "object.h"

namespace unity {
	class Texture : public Object {
	public:
		inline unsigned int texture_id() {
			const auto& object = mem::read<uintptr_t>(reinterpret_cast<uintptr_t>(this) + 0x10);
			if (!memory::is_valid(object))
				return 0;

			return mem::read<unsigned int>(object + 0x50);
		}
	};
}
