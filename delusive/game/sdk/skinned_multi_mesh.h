#pragma once
#include "../il2cpp/il2cpp.h"
#include "unity/list.h"
#include "unity/renderer.h"

namespace sdk {
	class SkinnedMultiMesh {
	private:
		static inline uintptr_t get_renderers_ptr = 0;

	public:
		static void initialize() {
			get_renderers_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_(""), _("SkinnedMultiMesh")), _("get_Renderers"), -1));
		}

		inline unity::c_list<unity::Renderer*>* renderers() {
			return safe.call<unity::c_list<unity::Renderer*>*>(get_renderers_ptr, this);
		}
	};
}
