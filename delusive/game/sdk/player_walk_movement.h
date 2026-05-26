#pragma once
#include "../il2cpp/il2cpp.h"
#include "base_movement.h"

namespace unity {
	class RigidBody;
	class CapsuleCollider;
}

namespace sdk {
    class BasePlayer;
    extern BasePlayer* local_player;

	class PlayerWalkMovement : public BaseMovement {
	private:
		static inline uintptr_t flying_ptr = 0;
		static inline uintptr_t body_ptr = 0;
		static inline uintptr_t capsule_ptr = 0;
		static inline uintptr_t ladder_ptr = 0;
		static inline uintptr_t running_ptr = 0;
		static inline uintptr_t ground_time_ptr = 0;
		static inline uintptr_t jumping_ptr = 0;
		static inline uintptr_t sliding_ptr = 0;
		static inline uintptr_t climbing_ptr = 0;
		static inline uintptr_t grounded_ptr = 0;
		static inline uintptr_t was_falling_ptr = 0;
		static inline uintptr_t land_time_ptr = 0;
		static inline uintptr_t jump_time_ptr = 0;
		static inline uintptr_t ground_angle_ptr = 0;
		static inline uintptr_t capsule_height_ptr = 0;
		static inline uintptr_t ground_angle_new_ptr = 0;
		static inline uintptr_t previous_velocity_ptr = 0;
		static inline uintptr_t gravity_multiplier_ptr = 0;
		static inline uintptr_t gravity_multiplier_swimming_ptr = 0;
		static inline uintptr_t capsule_center_ptr = 0;
		static inline uintptr_t admin_cheat_ptr = 0;
		static inline uintptr_t grounded_backing_ptr = 0;

		static inline uintptr_t teleport_to_ptr = 0;

	public:
		static void initialize() {
			il2cpp_field_offset(flying_ptr, _(""), _("PlayerWalkMovement"), _("flying"));
			il2cpp_field_offset(body_ptr, _(""), _("PlayerWalkMovement"), _("body"));
			il2cpp_field_offset(capsule_ptr, _(""), _("PlayerWalkMovement"), _("capsule"));
			il2cpp_field_offset(ladder_ptr, _(""), _("PlayerWalkMovement"), _("ladder"));
			il2cpp_field_offset(running_ptr, _(""), _("BaseMovement"), _("<Running>k__BackingField"));
			il2cpp_field_offset(grounded_backing_ptr, _(""), _("BaseMovement"), _("<Grounded>k__BackingField"));
			il2cpp_field_offset(ground_time_ptr, _(""), _("PlayerWalkMovement"), _("groundTime"));
			il2cpp_field_offset(jumping_ptr, _(""), _("PlayerWalkMovement"), _("jumping"));
			il2cpp_field_offset(sliding_ptr, _(""), _("PlayerWalkMovement"), _("sliding"));
			il2cpp_field_offset(climbing_ptr, _(""), _("PlayerWalkMovement"), _("climbing"));
			il2cpp_field_offset(grounded_ptr, _(""), _("PlayerWalkMovement"), _("grounded"));
			il2cpp_field_offset(was_falling_ptr, _(""), _("PlayerWalkMovement"), _("wasFalling"));
			il2cpp_field_offset(land_time_ptr, _(""), _("PlayerWalkMovement"), _("landTime"));
			il2cpp_field_offset(jump_time_ptr, _(""), _("PlayerWalkMovement"), _("jumpTime"));
			il2cpp_field_offset(ground_angle_ptr, _(""), _("PlayerWalkMovement"), _("groundAngle"));
			il2cpp_field_offset(capsule_height_ptr, _(""), _("PlayerWalkMovement"), _("capsuleHeight"));
			il2cpp_field_offset(ground_angle_new_ptr, _(""), _("PlayerWalkMovement"), _("groundAngleNew"));
			il2cpp_field_offset(previous_velocity_ptr, _(""), _("PlayerWalkMovement"), _("previousVelocity"));
			il2cpp_field_offset(gravity_multiplier_ptr, _(""), _("PlayerWalkMovement"), _("gravityMultiplier"));
			il2cpp_field_offset(gravity_multiplier_swimming_ptr, _(""), _("PlayerWalkMovement"), _("gravityMultiplierSwimming"));
			il2cpp_field_offset(capsule_center_ptr, _(""), _("PlayerWalkMovement"), _("capsuleCenter"));
			il2cpp_field_offset(admin_cheat_ptr, _(""), _("BaseMovement"), _("adminCheat"));

			teleport_to_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_(""), _("PlayerWalkMovement")), _("TeleportTo"), 2));
		}

		il2cpp_field_get(bool, flying, flying_ptr);
		il2cpp_field_get(unity::RigidBody*, body, body_ptr);
		il2cpp_field_get(unity::CapsuleCollider*, capsule, capsule_ptr);
		il2cpp_field_get(uintptr_t, ladder, ladder_ptr);
		il2cpp_field_get(float, running, running_ptr);
		il2cpp_field_get(float, ground_time, ground_time_ptr);
		il2cpp_field_get(bool, grounded, grounded_ptr);
		il2cpp_field_get(float, grounded_backing, grounded_backing_ptr);

		il2cpp_field_set(uintptr_t, set_ladder, ladder_ptr);
		il2cpp_field_set(bool, set_flying, flying_ptr);
		il2cpp_field_set(bool, set_jumping, jumping_ptr);
		il2cpp_field_set(bool, set_sliding, sliding_ptr);
		il2cpp_field_set(float, set_running, running_ptr);
		il2cpp_field_set(bool, set_climbing, climbing_ptr);
		il2cpp_field_set(bool, set_grounded, grounded_ptr);
		il2cpp_field_set(float, set_grounded_backing, grounded_backing_ptr);
		il2cpp_field_set(bool, set_was_falling, was_falling_ptr);
		il2cpp_field_set(float, set_land_time, land_time_ptr);
		il2cpp_field_set(float, set_jump_time, jump_time_ptr);
		il2cpp_field_set(float, set_ground_time, ground_time_ptr);
		il2cpp_field_set(float, set_ground_angle, ground_angle_ptr);
		il2cpp_field_set(float, set_capsule_height, capsule_height_ptr);
		il2cpp_field_set(float, set_ground_angle_new, ground_angle_new_ptr);
		il2cpp_field_set(vec3_t, set_previous_velocity, previous_velocity_ptr);
		il2cpp_field_set(float, set_gravity_multiplier, gravity_multiplier_ptr);
		il2cpp_field_set(float, set_gravity_multiplier_swimming, gravity_multiplier_swimming_ptr);
		il2cpp_field_set(float, set_capsule_center, capsule_center_ptr);
		il2cpp_field_set(bool, set_admin_cheat, admin_cheat_ptr);

		inline void teleport_to(vec3_t pos) {
			safe.call<void>(teleport_to_ptr, this, pos, sdk::local_player);
		}
	};
}
