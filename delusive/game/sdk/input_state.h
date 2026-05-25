#pragma once
#include "../il2cpp/il2cpp.h"
#include "input_message.h"

namespace sdk {
	class InputState {
	private:
		static inline uintptr_t current_ptr = 0;
		static inline uintptr_t is_down_ptr = 0;
		static inline uintptr_t was_just_pressed_ptr = 0;

	public:
		static void initialize() {
			auto klass = il2cpp::get_class(_(""), _("InputState"));
			il2cpp_field_offset(current_ptr, _(""), _("InputState"), _("current"));
			is_down_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("IsDown"), 1));
			was_just_pressed_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("WasJustPressed"), 1));
		}

		il2cpp_field_get(InputMessage*, current, current_ptr);

		inline bool is_down(int button) {
			return safe.call<bool>(is_down_ptr, this, button);
		}

		inline bool was_just_pressed(int button) {
			return safe.call<bool>(was_just_pressed_ptr, this, button);
		}
	};
}
