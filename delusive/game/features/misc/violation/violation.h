#pragma once
#include "../../../sdk/sdk.h"
#include "../../../configs/configs.h"

namespace core {
    extern volatile bool g_unloading;
}

namespace violation {

    namespace cfg {
        constexpr float k_kick_threshold     = 95.f;
        constexpr float k_fade_rate          = 15.f;
        constexpr float k_teleport_speed     = 12.f;
        constexpr float k_forgiveness        = 6.5f;
        constexpr float k_penalty_vertical   = 35.f;
        constexpr float k_penalty_horizontal = 20.f;
        constexpr float k_fall_threshold     = -1.5f;
    }

    inline float  g_violation_level     = 0.f;
    inline float  g_last_violation_time = 0.f;

    inline vec3_t g_safe_ground_pos     = { 0.f, 0.f, 0.f };
    inline bool   g_safe_pos_valid      = false;

    inline float  g_dist_vertical       = 0.f;
    inline float  g_dist_horizontal     = 0.f;

    inline vec3_t g_prev_pos            = { 0.f, 0.f, 0.f };
    inline bool   g_prev_pos_valid      = false;

    inline float  g_teleport_progress   = 1.f;
    inline vec3_t g_teleport_target     = { 0.f, 0.f, 0.f };

    inline float  g_indicator_pct       = 0.f;

    inline bool is_pos_valid(const vec3_t& p) {
        if (p.x == 0.f && p.y == 0.f && p.z == 0.f) return false;
        if (p.x != p.x || p.y != p.y || p.z != p.z)  return false;
        if (fabsf(p.x) > 8000.f || fabsf(p.z) > 8000.f) return false;
        return true;
    }

    inline float smoothstep(float t) {
        t = std::clamp(t, 0.f, 1.f);
        return t * t * (3.f - 2.f * t);
    }

    inline void add_violation(float penalty) {
        g_violation_level += penalty;
        g_last_violation_time = unity::Time::time();
    }

    inline void fade_violations(float dt) {
        float now = unity::Time::time();
        if (now - g_last_violation_time > 5.f)
            g_violation_level = (std::max)(0.f, g_violation_level - cfg::k_fade_rate * dt);
    }

    inline void tick() {
        if (!memory::is_valid(sdk::local_player) || core::g_unloading)
            return;

        if (!config::misc::exploits::exploits_anti_fly_hack_kick.value) {
            g_dist_vertical     = 0.f;
            g_dist_horizontal   = 0.f;
            g_violation_level   = 0.f;
            g_teleport_progress = 1.f;
            g_indicator_pct     = 0.f;
            g_prev_pos_valid    = false;
            return;
        }

        auto* player = sdk::local_player;

        auto* transform = player->transform();
        if (!memory::is_valid(transform)) return;

        auto* movement = player->walk_movement();
        if (!memory::is_valid(movement)) return;

        float dt = unity::Time::delta_time();
        if (dt <= 0.f || dt > 0.5f) return;

        if (g_teleport_progress < 1.f) {
            g_teleport_progress += dt * cfg::k_teleport_speed;
            g_teleport_progress  = std::clamp(g_teleport_progress, 0.f, 1.f);

            if (is_pos_valid(g_teleport_target)) {
                vec3_t cur = transform->position();
                float  t   = smoothstep(g_teleport_progress);
                movement->teleport_to(vec3_t(
                    cur.x + (g_teleport_target.x - cur.x) * t,
                    cur.y + (g_teleport_target.y - cur.y) * t,
                    cur.z + (g_teleport_target.z - cur.z) * t
                ));
            }

            g_indicator_pct = std::clamp(g_violation_level, 0.f, 100.f);
            return;
        }

                vec3_t pos   = transform->position();

        bool grounded = false;
        if (memory::is_valid(movement)) {
            grounded = movement->grounded();
        }

        grounded = grounded
                     || player->is_swimming()
                     || player->on_ladder()
                     || player->is_sleeping()
                     || memory::is_valid(player->mounted());

        if (grounded) {
            if (is_pos_valid(pos)) {
                g_safe_ground_pos = pos;
                g_safe_pos_valid  = true;
            }
            g_dist_vertical   = 0.f;
            g_dist_horizontal = 0.f;
            g_prev_pos        = pos;
            g_prev_pos_valid  = true;
            fade_violations(dt);
            g_indicator_pct   = std::clamp(g_violation_level, 0.f, 100.f);
            return;
        }

        if (!g_prev_pos_valid) {
            g_prev_pos       = pos;
            g_prev_pos_valid = true;
            g_indicator_pct  = std::clamp(g_violation_level, 0.f, 100.f);
            return;
        }

        vec3_t frame_delta = pos - g_prev_pos;
        g_prev_pos = pos;

        float frame_dy = frame_delta.y;
        float frame_dh = frame_delta.length_2d();

        if (frame_dy < cfg::k_fall_threshold * dt) {
            fade_violations(dt);
            g_indicator_pct = std::clamp(g_violation_level, 0.f, 100.f);
            return;
        }

        if (frame_dy > 0.f)
            g_dist_vertical += frame_dy;

        if (frame_dh > 0.f && frame_dy >= 0.f)
            g_dist_horizontal += frame_dh;

        float jump_h  = player->jump_height();
        float limit_v = jump_h + cfg::k_forgiveness;
        float limit_h = 5.f   + cfg::k_forgiveness;

        bool over_v = g_dist_vertical   > limit_v;
        bool over_h = g_dist_horizontal > limit_h;

        if (over_v || over_h) {
            add_violation(over_v ? cfg::k_penalty_vertical : cfg::k_penalty_horizontal);

            g_dist_vertical   = 0.f;
            g_dist_horizontal = 0.f;

            if (g_violation_level >= cfg::k_kick_threshold) {
                if (g_safe_pos_valid && is_pos_valid(g_safe_ground_pos)) {
                    g_teleport_target   = g_safe_ground_pos;
                    g_teleport_progress = 0.f;

                    movement->set_target_movement(vec3_t(0.f, 0.f, 0.f));
                    auto* body = movement->body();
                    if (memory::is_valid(body))
                        body->set_velocity(vec3_t(0.f, 0.f, 0.f));

                    g_violation_level = 0.f;
                }
                else {
                    g_violation_level = (std::max)(0.f, g_violation_level - 30.f);
                }
            }
        }

        fade_violations(dt);
        g_indicator_pct = std::clamp(g_violation_level, 0.f, 100.f);
    }

}