#pragma once
#include "../il2cpp/il2cpp.h"
#include "unity/string.h"

namespace sdk {
	class TranslatePhrase {
	private:
		static inline uintptr_t get_translated_ptr = 0;

	public:
		static void initialize() {
			auto klass = il2cpp::get_class(_(""), _("TokenisedPhrase"));
			
			if (klass) {
				get_translated_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("get_translated"), -1));
			}
		}

		inline unity::c_string* name() {
			return safe.call<unity::c_string*>(get_translated_ptr, this);
		}
	};
}
