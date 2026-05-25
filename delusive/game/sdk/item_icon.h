#pragma once
#include "../il2cpp/il2cpp.h"

namespace sdk {
	class ItemIcon {
	private:
		static inline uintptr_t queued_for_looting_ptr = 0;
		static inline uintptr_t run_timed_action_ptr = 0;

	public:
		static void initialize() {
			auto klass = il2cpp::get_class(_(""), _("ItemIcon"));
			il2cpp_field_offset(queued_for_looting_ptr, _(""), _("ItemIcon"), _("queuedForLooting"));
			run_timed_action_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("RunTimedAction"), -1));
		}

		il2cpp_field_get(bool, queued_for_looting, queued_for_looting_ptr);

		inline void run_timed_action() {
			safe.call<void>(run_timed_action_ptr, this);
		}
	};
}
