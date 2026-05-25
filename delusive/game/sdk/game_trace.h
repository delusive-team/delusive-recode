#pragma once
#include "../il2cpp/il2cpp.h"
#include "trace_info.h"
#include "unity/list.h"

namespace sdk {
	class GameTrace {
	private:
		static inline uintptr_t trace_all_ptr = 0;

	public:
		static void initialize() {
			auto klass = il2cpp::get_class(_(""), _("GameTrace"));
			trace_all_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("TraceAll"), 3));
		}

		static inline void trace_all(HitTest* hit_test, unity::c_list<trace_info_t>* list, int layer_mask) {
			safe.call<void>(trace_all_ptr, hit_test, list, layer_mask);
		}
	};
}
