#pragma once
#include "../../il2cpp/il2cpp.h"
#include "../enums/enums.h"
#include "../math/vector.h"
#include "string.h"

namespace unity {
	class Input {
	private:
		static inline uintptr_t get_key_ptr = 0;
		static inline uintptr_t get_any_key_ptr = 0;
		static inline uintptr_t get_axis_raw_ptr = 0;
		static inline uintptr_t get_key_down_ptr = 0;
		static inline uintptr_t get_mouse_position_ptr = 0;
		static inline uintptr_t get_composition_string_ptr = 0;
		static inline uintptr_t get_mouse_scroll_delta_injected_ptr = 0;

	public:
		static void initialize() {
			get_key_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Input")), _("GetKey"), 1));
			get_any_key_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Input")), _("get_anyKey"), -1));
			get_axis_raw_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Input")), _("GetAxisRaw"), 1));
			get_key_down_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Input")), _("GetKeyDown"), 1));
			get_mouse_position_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Input")), _("get_mousePosition"), -1));
			get_composition_string_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Input")), _("get_compositionString"), -1));
			
			get_mouse_scroll_delta_injected_ptr = il2cpp::resolve_icall(_("UnityEngine.Input::get_mouseScrollDelta_Injected(UnityEngine.Vector2&)"));
		}

		static bool any_key() {
			return safe.call<bool>(get_any_key_ptr);
		}

		static bool get_key(enums::e_key_code key) {
			return safe.call<bool>(get_key_ptr, key);
		}

		static bool get_key_down(enums::e_key_code key) {
			return safe.call<bool>(get_key_down_ptr, key);
		}

		static c_string* composition_string() {
			return safe.call<c_string*>(get_composition_string_ptr);
		}

		static vec2_t mouse_position() {
			return safe.call<vec2_t>(get_mouse_position_ptr);
		}

		static float axis_raw(const char* axis_name) {
			return safe.call<float>(get_axis_raw_ptr, il2cpp::new_string(axis_name));
		}

		static vec2_t mouse_scroll_delta() {
			vec2_t result;
			safe.call<void>(get_mouse_scroll_delta_injected_ptr, &result);
			return result;
		}
	};
}
