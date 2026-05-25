#pragma once
#include "../il2cpp/il2cpp.h"
#include "unity/list_dictionary.h"

namespace sdk {
	class EntityRealm {
	public:
		inline auto entity_list( )
		{
			return mem::read< unity::c_list_dictionary* >(this + 0x10);
		}
	};
}
