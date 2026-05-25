#pragma once
#include "../../il2cpp/il2cpp.h"
#include "object.h"
#include "texture.h"

namespace unity {
	class Sprite : public Object {
	private:
		static inline uintptr_t get_texture_ptr = 0;

	public:
		static void initialize() {
			get_texture_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Sprite")), _("get_texture"), -1));
		}

		inline Texture* texture() {
			return safe.call<Texture*>(get_texture_ptr, this);
		}
	};
}
