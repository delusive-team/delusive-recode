#pragma once
#include "../il2cpp/il2cpp.h"
#include "base_entity.h"

namespace sdk {
	class StashContainer : public BaseEntity {
	private:
		static inline uintptr_t is_hidden_ptr = 0;

	public:
		static void initialize() {
			is_hidden_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_(""), _("StashContainer")), _("IsHidden"), -1));
		}

		inline bool is_hidden() {
			return safe.call<bool>(is_hidden_ptr, this);
		}
	};
}
