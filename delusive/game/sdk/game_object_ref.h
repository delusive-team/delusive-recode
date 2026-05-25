#pragma once
#include "../il2cpp/il2cpp.h"
#include "unity/string.h"

namespace sdk {
	class GameObjectRef {
	public:
		inline unity::c_string* guid() {
			return mem::read<unity::c_string*>((uintptr_t)this + 0x10);
		}
	};
}
