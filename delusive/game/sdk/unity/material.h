#pragma once
#include "../../il2cpp/il2cpp.h"
#include "object.h"
#include "shader.h"
#include "color.h"

namespace unity {
	class Material : public Object {
	private:
		static inline uintptr_t set_int_ptr = 0;
		static inline uintptr_t get_name_ptr = 0;
		static inline uintptr_t set_float_ptr = 0;
		static inline uintptr_t set_color_ptr = 0;
		static inline uintptr_t get_shader_ptr = 0;
		static inline uintptr_t set_shader_ptr = 0;
		static inline uintptr_t set_texture_ptr = 0;

	public:
		static void initialize() {
			set_int_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Material")), _("SetInt"), 2));
			get_name_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Material")), _("get_name"), -1));
			set_float_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Material")), _("SetFloat"), 2));
			set_color_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Material")), _("SetColor"), 2));
			get_shader_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Material")), _("get_shader"), -1));
			set_shader_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Material")), _("set_shader"), 1));
			set_texture_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Material")), _("SetTexture"), 2));
		}

		inline c_string* name() {
			return safe.call<c_string*>(get_name_ptr, this);
		}

		inline Shader* shader() {
			return safe.call<Shader*>(get_shader_ptr, this);
		}

		inline void set_shader(Shader* shader) {
			safe.call<void>(set_shader_ptr, this, shader);
		}

		inline void set_int(const char* prop, int value) {
			safe.call<void>(set_int_ptr, this, il2cpp::new_string(prop), value);
		}

		inline void set_float(const char* prop, float value) {
			safe.call<void>(set_float_ptr, this, il2cpp::new_string(prop), value);
		}

		inline void set_color(const char* prop, c_color color) {
			safe.call<void>(set_color_ptr, this, il2cpp::new_string(prop), color);
		}

		inline void set_texture(const char* prop, uintptr_t texture) {
			safe.call<void>(set_texture_ptr, this, il2cpp::new_string(prop), texture);
		}
	};
}
