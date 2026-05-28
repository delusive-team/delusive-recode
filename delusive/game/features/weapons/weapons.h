#pragma once
#include "../../sdk/sdk.h"
#include "../../../configs/configs.h"

namespace core {
    extern volatile bool g_unloading;
}

struct WeaponOriginals {
    float aimCone = -1.f;
    float hipAimCone = -1.f;
    float aimConePenaltyMax = -1.f;
    float aimconePenaltyPerShot = -1.f;
    float aimSway = -1.f;
    float aimSwaySpeed = -1.f;
    float recoilYawMin = -1.f;
    float recoilYawMax = -1.f;
    float recoilPitchMin = -1.f;
    float recoilPitchMax = -1.f;
    float stancePenaltyScale = -1.f;
    float repeatDelay = -1.f;
    bool  automatic = false;
    float deployDelay = -1.f;
    void* weapon_ptr = nullptr;

    bool valid() const { return weapon_ptr != nullptr && aimCone >= 0.f; }
    void invalidate() { weapon_ptr = nullptr; aimCone = -1.f; deployDelay = -1.f; repeatDelay = -1.f; }
};

namespace features {
    class c_weapons {
    public:
        static inline c_weapons& get() {
            static c_weapons inst;
            return inst;
        }

        void run(sdk::BasePlayer* local);
        void on_unload(sdk::BasePlayer* local);

    private:
        WeaponOriginals m_orig;

        bool cache_originals(sdk::HeldEntity* weapon);

        void apply_spread_weapon(sdk::HeldEntity* weapon);
        void apply_spread_projectile(sdk::HeldEntity* weapon);
        void apply_sway(sdk::HeldEntity* weapon);
        void apply_recoil(sdk::HeldEntity* weapon);

        void restore_spread_weapon(sdk::HeldEntity* weapon);
        void restore_spread_projectile(sdk::HeldEntity* weapon);
        void restore_sway(sdk::HeldEntity* weapon);
        void restore_recoil(sdk::HeldEntity* weapon);
    };

    inline sdk::RecoilProperties* get_recoil_props(sdk::HeldEntity* weapon) {
        auto* rp = weapon->recoil();
        return memory::is_valid(rp) ? rp : nullptr;
    }

    inline bool c_weapons::cache_originals(sdk::HeldEntity* weapon) {
        if (!memory::is_valid(weapon)) return false;

        m_orig.aimCone = weapon->aim_cone();
        m_orig.hipAimCone = weapon->hip_aim_cone();
        m_orig.aimConePenaltyMax = weapon->aim_cone_penalty_max();
        m_orig.aimconePenaltyPerShot = weapon->aimcone_penalty_per_shot();
        m_orig.aimSway = weapon->aim_sway();
        m_orig.aimSwaySpeed = weapon->aim_sway_speed();
        m_orig.repeatDelay = weapon->repeat_delay();

        bool is_flint = weapon->is_class(_("FlintStrikeWeapon"));
        auto* rp = is_flint ? nullptr : get_recoil_props(weapon);
        if (rp) {
            m_orig.recoilYawMin  = rp->recoil_yaw_min();
            m_orig.recoilYawMax  = rp->recoil_yaw_max();
            m_orig.recoilPitchMin = rp->recoil_pitch_min();
            m_orig.recoilPitchMax = rp->recoil_pitch_max();
        }

        m_orig.stancePenaltyScale = weapon->stance_penalty_scale();
        m_orig.automatic = weapon->automatic();
        m_orig.deployDelay = weapon->deploy_delay();
        m_orig.weapon_ptr = weapon;
        return true;
    }

    inline void c_weapons::apply_spread_weapon(sdk::HeldEntity* weapon) {
        float cone = config::weapons::no_spread_weapon_value.value / 100.f;
        weapon->set_aim_cone(cone);
        weapon->set_hip_aim_cone(cone);
        weapon->set_aim_cone_penalty_max(0.f);
        weapon->set_aimcone_penalty_per_shot(0.f);
    }

    inline void c_weapons::apply_spread_projectile(sdk::HeldEntity* weapon) {
        float scale = config::weapons::no_spread_projectile_value.value / 100.f;
        weapon->set_stance_penalty_scale(m_orig.stancePenaltyScale * scale);
    }

    inline void c_weapons::apply_sway(sdk::HeldEntity* weapon) {
        float sway = config::weapons::no_sway_value.value / 100.f;
        weapon->set_aim_sway(m_orig.aimSway * sway);
        weapon->set_aim_sway_speed(m_orig.aimSwaySpeed * sway);
    }

    inline void c_weapons::apply_recoil(sdk::HeldEntity* weapon) {
        if (weapon->is_class(_("FlintStrikeWeapon"))) return;
        auto* rp = get_recoil_props(weapon);
        if (!rp) return;

        if (config::weapons::recoil_mode.value == 0) {
            float scale = config::weapons::recoil_amount.value / 100.f;
            rp->set_recoil_yaw_min(m_orig.recoilYawMin * scale);
            rp->set_recoil_yaw_max(m_orig.recoilYawMax * scale);
            rp->set_recoil_pitch_min(m_orig.recoilPitchMin * scale);
            rp->set_recoil_pitch_max(m_orig.recoilPitchMax * scale);
        }
        else {
            float scale_x = config::weapons::recoil_x.value / 100.f;  // pitch 
            float scale_y = config::weapons::recoil_y.value / 100.f;  // yaw  
            rp->set_recoil_yaw_min(m_orig.recoilYawMin * scale_y);
            rp->set_recoil_yaw_max(m_orig.recoilYawMax * scale_y);
            rp->set_recoil_pitch_min(m_orig.recoilPitchMin * scale_x);
            rp->set_recoil_pitch_max(m_orig.recoilPitchMax * scale_x);
        }
    }

    inline void c_weapons::restore_spread_weapon(sdk::HeldEntity* weapon) {
        if (!m_orig.valid()) return;
        weapon->set_aim_cone(m_orig.aimCone);
        weapon->set_hip_aim_cone(m_orig.hipAimCone);
        weapon->set_aim_cone_penalty_max(m_orig.aimConePenaltyMax);
        weapon->set_aimcone_penalty_per_shot(m_orig.aimconePenaltyPerShot);
    }

    inline void c_weapons::restore_spread_projectile(sdk::HeldEntity* weapon) {
        if (!m_orig.valid()) return;
        weapon->set_stance_penalty_scale(m_orig.stancePenaltyScale);
    }

    inline void c_weapons::restore_sway(sdk::HeldEntity* weapon) {
        if (!m_orig.valid()) return;
        weapon->set_aim_sway(m_orig.aimSway);
        weapon->set_aim_sway_speed(m_orig.aimSwaySpeed);
    }

    inline void c_weapons::restore_recoil(sdk::HeldEntity* weapon) {
        if (!m_orig.valid()) return;
        if (weapon->is_class(_("FlintStrikeWeapon"))) return;
        auto* rp = get_recoil_props(weapon);
        if (!rp) return;
        rp->set_recoil_yaw_min(m_orig.recoilYawMin);
        rp->set_recoil_yaw_max(m_orig.recoilYawMax);
        rp->set_recoil_pitch_min(m_orig.recoilPitchMin);
        rp->set_recoil_pitch_max(m_orig.recoilPitchMax);
    }

    inline void c_weapons::run(sdk::BasePlayer* local) {
        if (core::g_unloading) return;

        if (!memory::is_valid(local) || local->lifestate() != 0) return;

        auto* weapon = local->held_entity();
        if (!memory::is_valid(weapon)) {
            m_orig.invalidate();
            return;
        }

        // Verify it is a valid IL2CPP object and check its class
        uintptr_t vtable = mem::read<uintptr_t>((uintptr_t)weapon);
        if (!memory::is_valid(vtable)) {
            static uintptr_t last_invalid_ptr = 0;
            if (last_invalid_ptr != (uintptr_t)weapon) {
                LOG_ERROR(_("[Weapons] Held entity %p has invalid vtable %p"), weapon, (void*)vtable);
                last_invalid_ptr = (uintptr_t)weapon;
            }
            return;
        }

        bool is_projectile_weapon = weapon->is_class(_("BaseProjectile")) ||
                                    weapon->is_class(_("BowWeapon")) ||
                                    weapon->is_class(_("CompoundBowWeapon")) ||
                                    weapon->is_class(_("CrossbowWeapon")) ||
                                    weapon->is_class(_("FlintStrikeWeapon"));

        if (!is_projectile_weapon) {
            m_orig.invalidate();
            return;
        }

        if (m_orig.weapon_ptr != weapon) {
            cache_originals(weapon);
        }

        if (config::weapons::no_spread_weapon.value)
            apply_spread_weapon(weapon);
        else
            restore_spread_weapon(weapon);

        if (config::weapons::no_spread_projectile.value)
            apply_spread_projectile(weapon);
        else
            restore_spread_projectile(weapon);

        if (config::weapons::no_sway.value)
            apply_sway(weapon);
        else
            restore_sway(weapon);

        if (config::weapons::no_recoil.value)
            apply_recoil(weapon);
        else
            restore_recoil(weapon);

        // Full Auto
        if (config::weapons::full_auto.value && !weapon->is_class(_("FlintStrikeWeapon")))
            weapon->set_automatic(true);

        // No Deploy Delay
        if (config::weapons::no_deploy_delay.value) {
            weapon->set_deploy_delay(0.f);
            weapon->set_time_since_deploy(999.f);
        }

        // Weapon Spam — reduce repeat delay to fire faster
        if (config::weapons::weapon_spam.value &&
            !weapon->is_class(_("FlintStrikeWeapon")))
        {
            int key = config::weapons::weapon_spam_key.value;
            int mode = config::weapons::weapon_spam_key_mode.value;
            bool active = false;
            if (key != 0) {
                active = (mode == 0) ? (GetAsyncKeyState(key) & 0x8000)
                    : (mode == 1) ? (GetKeyState(key) & 0x0001) : true;
            }
            else {
                active = (mode == 2);
            }

            if (active) {
                float delay = (std::max)(config::weapons::weapon_spam_delay.value / 1000.f, 0.001f);
                weapon->set_repeat_delay(delay);
            }
            else if (m_orig.repeatDelay >= 0.f) {
                weapon->set_repeat_delay(m_orig.repeatDelay);
            }
        }
        else if (m_orig.repeatDelay >= 0.f) {
            weapon->set_repeat_delay(m_orig.repeatDelay);
        }

        // Insta Eoka
        if (config::weapons::insta_eoka.value && weapon->is_class(_("FlintStrikeWeapon"))) {
            float chance = config::weapons::eoka_chance.value / 100.f;
            weapon->set_success_fraction(chance);
            weapon->set_did_spark_this_frame(true);
        }
    }

    inline void c_weapons::on_unload(sdk::BasePlayer* local) {
        if (!m_orig.valid() || !memory::is_valid(local)) return;

        auto* weapon = local->held_entity();
        if (!memory::is_valid(weapon) || weapon != m_orig.weapon_ptr) return;

        restore_spread_weapon(weapon);
        restore_spread_projectile(weapon);
        restore_sway(weapon);
        restore_recoil(weapon);

        // Restore Full Auto
        weapon->set_automatic(m_orig.automatic);

        // Restore Deploy Delay
        if (m_orig.deployDelay >= 0.f)
            weapon->set_deploy_delay(m_orig.deployDelay);

        // Restore Repeat Delay
        if (m_orig.repeatDelay >= 0.f)
            weapon->set_repeat_delay(m_orig.repeatDelay);

        m_orig.invalidate();
    }
}
