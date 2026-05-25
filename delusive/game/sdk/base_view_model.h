#pragma once
#include "../il2cpp/il2cpp.h"
#include "unity/behaviour.h"

namespace sdk {
	class ViewmodelBob;
	class ViewmodelLower;
	class ViewmodelSway;
	class Animator;

	class BaseViewModel : public unity::Behaviour {
	private:
		static inline uintptr_t use_view_model_camera_ptr = 0;
		static inline uintptr_t get_active_model_ptr = 0;

		static inline uintptr_t bob_model_ptr = 0;
		static inline uintptr_t lower_model_ptr = 0;
		static inline uintptr_t sway_model_ptr = 0;
		static inline uintptr_t animator_model_ptr = 0;


	public:
		static void initialize() {

			il2cpp_field_offset(bob_model_ptr, _(""), _("BaseViewModel"), _("bob"));
			il2cpp_field_offset(lower_model_ptr, _(""), _("BaseViewModel"), _("lower"));
			il2cpp_field_offset(sway_model_ptr, _(""), _("BaseViewModel"), _("sway"));
			il2cpp_field_offset(animator_model_ptr, _(""), _("BaseViewModel"), _("animator"));

			il2cpp_field_offset(use_view_model_camera_ptr, _(""), _("BaseViewModel"), _("useViewModelCamera"));
			get_active_model_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_(""), _("BaseViewModel")), _("get_ActiveModel"), -1));
		}

		il2cpp_field_set(bool, set_use_view_model_camera, use_view_model_camera_ptr);

		il2cpp_field_get(ViewmodelBob*, bob_model, bob_model_ptr);
		il2cpp_field_get(ViewmodelLower*, lower_model, lower_model_ptr);
		il2cpp_field_get(ViewmodelSway*, sway_model, sway_model_ptr);
		il2cpp_field_get(Animator*, animator, animator_model_ptr);

		static BaseViewModel* active_model() {
			return safe.call<BaseViewModel*>(get_active_model_ptr);
		}
	};
}
