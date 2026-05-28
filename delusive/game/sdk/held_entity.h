#pragma once
#include "../il2cpp/il2cpp.h"
#include "base_entity.h"
#include "recoil_properties.h"
#include "magazine.h"
#include "projectile.h"
#include "unity/list.h"
#include "enums/enums.h"
#include "unity/time.h"

namespace sdk {
	class ViewModel;

	class HeldEntity : public BaseEntity {
	private:
		// Fields
		static inline uintptr_t recoil_ptr = 0;
		static inline uintptr_t aim_sway_ptr = 0;
		static inline uintptr_t aim_cone_ptr = 0;
		static inline uintptr_t distance_ptr = 0;
		static inline uintptr_t automatic_ptr = 0;
		static inline uintptr_t view_model_ptr = 0;
		static inline uintptr_t was_aiming_ptr = 0;
		static inline uintptr_t reload_time_ptr = 0;
		static inline uintptr_t is_deployed_ptr = 0;
		static inline uintptr_t needs_cycle_ptr = 0;
		static inline uintptr_t attack_ready_ptr = 0;
		static inline uintptr_t manual_cycle_ptr = 0;
		static inline uintptr_t repeat_delay_ptr = 0;
		static inline uintptr_t deploy_delay_ptr = 0;
		static inline uintptr_t max_distance_ptr = 0;
		static inline uintptr_t block_sprint_ptr = 0;
		static inline uintptr_t hip_aim_cone_ptr = 0;
		static inline uintptr_t attack_radius_ptr = 0;
		static inline uintptr_t trigger_ready_ptr = 0;
		static inline uintptr_t aim_sway_speed_ptr = 0;
		static inline uintptr_t distance_scale_ptr = 0;
		static inline uintptr_t stance_penalty_ptr = 0;
		static inline uintptr_t last_shot_time_ptr = 0;
		static inline uintptr_t aimcone_penalty_ptr = 0;
		static inline uintptr_t primary_magazine_ptr = 0;
		static inline uintptr_t success_fraction_ptr = 0;
		static inline uintptr_t next_reload_time_ptr = 0;
		static inline uintptr_t next_attack_time_ptr = 0;
		static inline uintptr_t damage_properties_ptr = 0;
		static inline uintptr_t time_since_deploy_ptr = 0;
		static inline uintptr_t string_bonus_damage_ptr = 0;
		static inline uintptr_t created_projectiles_ptr = 0;
		static inline uintptr_t aim_cone_penalty_max_ptr = 0;
		static inline uintptr_t did_spark_this_frame_ptr = 0;
		static inline uintptr_t stance_penalty_scale_ptr = 0;
		static inline uintptr_t current_hold_progress_ptr = 0;
		static inline uintptr_t no_aiming_while_cycling_ptr = 0;
		static inline uintptr_t aimcone_penalty_per_shot_ptr = 0;
		static inline uintptr_t projectile_velocity_scale_ptr = 0;
		static inline uintptr_t aimcone_penalty_recover_time_ptr = 0;
		static inline uintptr_t aimcone_penalty_recover_delay_ptr = 0;
		static inline uintptr_t movement_penalty_ramp_up_time_ptr = 0;

		// Methods
		static inline uintptr_t do_throw_ptr = 0;
		static inline uintptr_t shot_fired_ptr = 0;
		static inline uintptr_t begin_cycle_ptr = 0;
		static inline uintptr_t process_attack_ptr = 0;
		static inline uintptr_t is_fully_delpoyed_ptr = 0;
		static inline uintptr_t launch_projectile_ptr = 0;
		static inline uintptr_t has_attack_cooldown_ptr = 0;
		static inline uintptr_t update_ammo_display_ptr = 0;
		static inline uintptr_t start_attack_cooldown_ptr = 0;
		static inline uintptr_t did_attack_clientside_ptr = 0;
		static inline uintptr_t get_owner_item_definition_ptr = 0;
		static inline uintptr_t get_projectile_velocity_scale_ptr = 0;

	public:
		static void initialize() {
			il2cpp_field_offset(recoil_ptr, _(""), _("BaseProjectile"), _("recoil"));
			il2cpp_field_offset(aim_sway_ptr, _(""), _("BaseProjectile"), _("aimSway"));
			il2cpp_field_offset(aim_cone_ptr, _(""), _("BaseProjectile"), _("aimCone"));
			il2cpp_field_offset(distance_ptr, _(""), _("BaseMelee"), _("maxDistance"));
			il2cpp_field_offset(automatic_ptr, _(""), _("BaseProjectile"), _("automatic"));
			il2cpp_field_offset(view_model_ptr, _(""), _("HeldEntity"), _("viewModel"));
			il2cpp_field_offset(was_aiming_ptr, _(""), _("BowWeapon"), _("wasAiming"));
			il2cpp_field_offset(reload_time_ptr, _(""), _("BaseProjectile"), _("reloadTime"));
			il2cpp_field_offset(is_deployed_ptr, _(""), _("HeldEntity"), _("isDeployed"));
			il2cpp_field_offset(needs_cycle_ptr, _(""), _("BaseProjectile"), _("needsCycle"));
			il2cpp_field_offset(attack_ready_ptr, _(""), _("BowWeapon"), _("attackReady"));
			il2cpp_field_offset(manual_cycle_ptr, _(""), _("BaseProjectile"), _("manualCycle"));
			il2cpp_field_offset(repeat_delay_ptr, _(""), _("AttackEntity"), _("repeatDelay"));
			il2cpp_field_offset(deploy_delay_ptr, _(""), _("AttackEntity"), _("deployDelay"));
			il2cpp_field_offset(max_distance_ptr, _(""), _("BaseMelee"), _("maxDistance"));
			il2cpp_field_offset(block_sprint_ptr, _(""), _("BaseMelee"), _("blockSprintOnAttack"));
			il2cpp_field_offset(hip_aim_cone_ptr, _(""), _("BaseProjectile"), _("hipAimCone"));
			il2cpp_field_offset(attack_radius_ptr, _(""), _("BaseMelee"), _("attackRadius"));
			il2cpp_field_offset(trigger_ready_ptr, _(""), _("BaseProjectile"), _("triggerReady"));
			il2cpp_field_offset(aim_sway_speed_ptr, _(""), _("BaseProjectile"), _("aimSwaySpeed"));
			il2cpp_field_offset(distance_scale_ptr, _(""), _("BaseProjectile"), _("distanceScale"));
			il2cpp_field_offset(stance_penalty_ptr, _(""), _("BaseProjectile"), _("stancePenalty"));
			il2cpp_field_offset(last_shot_time_ptr, _(""), _("BaseProjectile"), _("lastShotTime"));
			il2cpp_field_offset(aimcone_penalty_ptr, _(""), _("BaseProjectile"), _("aimconePenalty"));
			il2cpp_field_offset(primary_magazine_ptr, _(""), _("BaseProjectile"), _("primaryMagazine"));
			il2cpp_field_offset(success_fraction_ptr, _(""), _("FlintStrikeWeapon"), _("successFraction"));
			il2cpp_field_offset(next_reload_time_ptr, _(""), _("BaseProjectile"), _("nextReloadTime"));
			il2cpp_field_offset(next_attack_time_ptr, _(""), _("AttackEntity"), _("nextAttackTime"));
			il2cpp_field_offset(damage_properties_ptr, _(""), _("BaseMelee"), _("damageProperties"));
			il2cpp_field_offset(time_since_deploy_ptr, _(""), _("AttackEntity"), _("timeSinceDeploy"));
			il2cpp_field_offset(string_bonus_damage_ptr, _(""), _("CompoundBowWeapon"), _("stringBonusDamage"));
			il2cpp_field_offset(created_projectiles_ptr, _(""), _("BaseProjectile"), _("createdProjectiles"));
			il2cpp_field_offset(aim_cone_penalty_max_ptr, _(""), _("BaseProjectile"), _("aimConePenaltyMax"));
			il2cpp_field_offset(did_spark_this_frame_ptr, _(""), _("FlintStrikeWeapon"), _("_didSparkThisFrame"));
			il2cpp_field_offset(stance_penalty_scale_ptr, _(""), _("BaseProjectile"), _("stancePenaltyScale"));
			il2cpp_field_offset(current_hold_progress_ptr, _(""), _("CompoundBowWeapon"), _("currentHoldProgress"));
			il2cpp_field_offset(no_aiming_while_cycling_ptr, _(""), _("BaseProjectile"), _("noAimingWhileCycling"));
			il2cpp_field_offset(aimcone_penalty_per_shot_ptr, _(""), _("BaseProjectile"), _("aimconePenaltyPerShot"));
			il2cpp_field_offset(projectile_velocity_scale_ptr, _(""), _("BaseProjectile"), _("projectileVelocityScale"));
			il2cpp_field_offset(aimcone_penalty_recover_time_ptr, _(""), _("BaseProjectile"), _("aimconePenaltyRecoverTime"));
			il2cpp_field_offset(aimcone_penalty_recover_delay_ptr, _(""), _("BaseProjectile"), _("aimconePenaltyRecoverDelay"));
			il2cpp_field_offset(movement_penalty_ramp_up_time_ptr, _(""), _("CompoundBowWeapon"), _("movementPenaltyRampUpTime"));

			do_throw_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_(""), _("BaseMelee")), _("DoThrow"), -1));
			shot_fired_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_(""), _("BaseProjectile")), _("ShotFired"), -1));
			begin_cycle_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_(""), _("BaseProjectile")), _("BeginCycle"), -1));
			process_attack_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_(""), _("BaseMelee")), _("ProcessAttack"), 1));
			is_fully_delpoyed_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_(""), _("AttackEntity")), _("IsFullyDeployed"), -1));
			launch_projectile_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_(""), _("BaseProjectile")), _("LaunchProjectile"), -1));
			has_attack_cooldown_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_(""), _("AttackEntity")), _("HasAttackCooldown"), -1));
			update_ammo_display_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_(""), _("BaseProjectile")), _("UpdateAmmoDisplay"), -1));
			start_attack_cooldown_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_(""), _("AttackEntity")), _("StartAttackCooldown"), 1));
			did_attack_clientside_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_(""), _("BaseProjectile")), _("DidAttackClientside"), -1));
			get_owner_item_definition_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_(""), _("HeldEntity")), _("GetOwnerItemDefinition"), -1));
			get_projectile_velocity_scale_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_(""), _("BaseProjectile")), _("GetProjectileVelocityScale"), 1));
		}

		il2cpp_field_get(ViewModel*, view_model, view_model_ptr);
		il2cpp_field_get(bool, is_deployed, is_deployed_ptr);
		il2cpp_field_get(float, next_reload_time, next_reload_time_ptr);
		il2cpp_field_set(float, set_next_reload_time, next_reload_time_ptr);
		il2cpp_field_get(float, last_shot_time, last_shot_time_ptr);
		il2cpp_field_get(bool, needs_cycle, needs_cycle_ptr);
		il2cpp_field_get(bool, manual_cycle, manual_cycle_ptr);
		il2cpp_field_get(bool, automatic, automatic_ptr);

		il2cpp_field_get(float, repeat_delay, repeat_delay_ptr);
		il2cpp_field_get(float, deploy_delay, deploy_delay_ptr);
		il2cpp_field_get(float, next_attack_time, next_attack_time_ptr);
		il2cpp_field_get(float, time_since_deploy, time_since_deploy_ptr);

		il2cpp_field_set(float, set_deploy_delay, deploy_delay_ptr);
		il2cpp_field_set(float, set_block_sprint, block_sprint_ptr); // Float, as requested

		il2cpp_field_get(float, max_distance, max_distance_ptr);
		il2cpp_field_set(float, set_max_distance, max_distance_ptr);
		il2cpp_field_set(float, set_attack_radius, attack_radius_ptr);

		il2cpp_field_set(bool, set_was_aiming, was_aiming_ptr);
		il2cpp_field_set(bool, set_attack_ready, attack_ready_ptr);

		il2cpp_field_set(float, set_string_bonus_damage, string_bonus_damage_ptr);
		il2cpp_field_set(float, set_current_hold_progress, current_hold_progress_ptr);
		il2cpp_field_set(float, set_movement_penalty_ramp_up_time, movement_penalty_ramp_up_time_ptr);

		il2cpp_field_set(float, set_repeat_delay, repeat_delay_ptr);

		il2cpp_field_set(float, set_success_fraction, success_fraction_ptr);
		il2cpp_field_set(bool, set_did_spark_this_frame, did_spark_this_frame_ptr);

		il2cpp_field_get(float, reload_time, reload_time_ptr);
		il2cpp_field_get(RecoilProperties*, recoil, recoil_ptr);
		il2cpp_field_get(Magazine*, primary_magazine, primary_magazine_ptr);
		il2cpp_field_get(uintptr_t, damage_properties, damage_properties_ptr);

		il2cpp_field_get(float, aim_cone, aim_cone_ptr);
		il2cpp_field_get(float, aim_sway, aim_sway_ptr);
		il2cpp_field_get(float, hip_aim_cone, hip_aim_cone_ptr);
		il2cpp_field_get(float, aim_cone_penalty_max, aim_cone_penalty_max_ptr);
		il2cpp_field_get(float, aimcone_penalty_per_shot, aimcone_penalty_per_shot_ptr);
		il2cpp_field_get(float, aim_sway_speed, aim_sway_speed_ptr);
		il2cpp_field_get(float, stance_penalty_scale, stance_penalty_scale_ptr);

		il2cpp_field_set(float, set_aim_cone, aim_cone_ptr);
		il2cpp_field_set(float, set_aim_sway, aim_sway_ptr);
		il2cpp_field_set(float, set_hip_aim_cone, hip_aim_cone_ptr);
		il2cpp_field_set(bool, set_automatic, automatic_ptr);
		il2cpp_field_set(bool, set_trigger_ready, trigger_ready_ptr);
		il2cpp_field_set(bool, set_needs_cycle, needs_cycle_ptr);
		il2cpp_field_set(float, set_aim_sway_speed, aim_sway_speed_ptr);
		il2cpp_field_set(float, set_distance_scale, distance_scale_ptr);
		il2cpp_field_set(float, set_stance_penalty, stance_penalty_ptr);
		il2cpp_field_set(float, set_aimcone_penalty, aimcone_penalty_ptr);
		il2cpp_field_set(float, set_aim_cone_penalty_max, aim_cone_penalty_max_ptr);
		il2cpp_field_set(float, set_stance_penalty_scale, stance_penalty_scale_ptr);
		il2cpp_field_set(float, set_no_aiming_while_cycling, no_aiming_while_cycling_ptr);
		il2cpp_field_set(float, set_aimcone_penalty_per_shot, aimcone_penalty_per_shot_ptr);
		il2cpp_field_set(float, set_projectile_velocity_scale, projectile_velocity_scale_ptr);
		il2cpp_field_set(float, set_aimcone_penalty_recover_time, aimcone_penalty_recover_time_ptr);
		il2cpp_field_set(float, set_aimcone_penalty_recover_delay, aimcone_penalty_recover_delay_ptr);
		il2cpp_field_set(float, set_time_since_deploy, time_since_deploy_ptr);

		il2cpp_field_get(unity::c_list<Projectile*>*, created_projectiles, created_projectiles_ptr);

		inline void do_throw() {
			safe.call<void>(do_throw_ptr, this);
		}

		inline void process_attack(HitTest* hit) {
			safe.call<void>(process_attack_ptr, this, hit);
		}

		inline void start_attack_cooldown(float cooldown) {
			safe.call<void>(start_attack_cooldown_ptr, this, cooldown);
		}

		inline ItemDefinition* owner_item_definition() {
			return safe.call<ItemDefinition*>(get_owner_item_definition_ptr, this);
		}

		inline bool has_attack_cooldown() {
			return safe.call<bool>(has_attack_cooldown_ptr, this);
		}

		inline bool has_reload_cooldown() {
			return unity::Time::time() < next_reload_time();
		}

		inline bool is_fully_deployed() {
			return safe.call<bool>(is_fully_delpoyed_ptr, this);
		}

		inline float projectile_velocity_scale(bool max = false) {
			return safe.call<float>(get_projectile_velocity_scale_ptr, this, max);
		}

		inline void shot_fired() {
			safe.call<void>(shot_fired_ptr, this);
		}

		inline void did_attack_clientside() {
			safe.call<void>(did_attack_clientside_ptr, this);
		}

		inline void begin_cycle() {
			safe.call<void>(begin_cycle_ptr, this);
		}

		inline void launch_projectile() {
			safe.call<void>(launch_projectile_ptr, this);
		}

		inline void update_ammo_display() {
			safe.call<void>(update_ammo_display_ptr, this);
		}

		inline void do_attack_recreation() {
			if (!is_class(_("BaseProjectile")))
				return;

			if (has_attack_cooldown())
				return;

			if (time_since_deploy() < deploy_delay())
				return;

			set_trigger_ready(true);

			if (manual_cycle() && needs_cycle()) {
				begin_cycle();
				return;
			}

			if (has_attack_cooldown())
				return;

			if (!automatic())
				set_trigger_ready(false);

			if (primary_magazine()->contents() <= 0)
				return;

			send_signal_broadcast(enums::e_signal::attack);

			primary_magazine()->set_contents(primary_magazine()->contents() - 1);

			launch_projectile();
			update_ammo_display();
			shot_fired();
			did_attack_clientside();
			if (!manual_cycle()) {
				begin_cycle();
				return;
			}

			set_needs_cycle(true);
		}
	};
}
