#pragma once
#include "../../sdk/sdk.h"
#include "../../../configs/configs.h"
#include "../../sdk/game_physics.h"
#include "../prediction/prediction.h"
#include <cmath>
#include <algorithm>
#include <vector>

namespace aimbot {
    namespace legitbot {

        struct aimbot_target_t {
            sdk::BasePlayer* player{ nullptr };
            int              bone_id{ 0 };
            vec3_t           position{};
            vec3_t           velocity{};
            bool             is_visible{ false };
            float            distance{ FLT_MAX };
            float            fov{ FLT_MAX };
        };

        static inline ULONGLONG        g_target_acquire_time = 0;
        static inline sdk::BasePlayer* g_last_target = nullptr;
        static inline int              g_last_bone = -1;
        static inline ULONGLONG        g_kill_time = 0;
        static inline ULONGLONG        g_aim_start_time = 0;
        static inline ULONGLONG        g_last_shot_time = 0;
        static inline bool             g_was_lbutton_down = false;

        static inline vec3_t           g_pos_history[3]{};
        static inline int              g_pos_idx = 0;
        static inline sdk::BasePlayer* g_pos_last_target = nullptr;

        static inline vec2_t           g_last_recoil{};
        static inline bool             g_rcs_initialized = false;

        inline void normalize_angles(float& pitch, float& yaw) {
            pitch = std::clamp(pitch, -89.f, 89.f);
            while (yaw > 180.f) yaw -= 360.f;
            while (yaw < -180.f) yaw += 360.f;
        }

        inline vec2_t calc_angle(const vec3_t& src, const vec3_t& dst) {
            vec3_t dir = dst - src;
            float  hyp = std::sqrt(dir.x * dir.x + dir.z * dir.z);
            if (hyp < 0.001f) hyp = 0.001f;
            float pitch = -std::atan2f(dir.y, hyp) * 57.2957795f;
            float yaw = std::atan2f(dir.x, dir.z) * 57.2957795f;
            normalize_angles(pitch, yaw);
            return vec2_t(pitch, yaw);
        }

        inline void apply_smooth(vec2_t& current, const vec2_t& target, float factor) {
            if (factor <= 1.f) { current = target; return; }

            vec2_t delta = { target.x - current.x, target.y - current.y };
            delta.y = std::remainder(delta.y, 360.f);

            if (std::abs(delta.x) < 0.05f && std::abs(delta.y) < 0.05f) {
                current = target;
            }
            else {
                current.x += delta.x / factor;
                current.y += delta.y / factor;
            }

            normalize_angles(current.x, current.y);
        }

        inline vec2_t get_view_angles(sdk::BasePlayer* local) {
            if (!memory::is_valid(local)) return {};
            auto* input = local->input();
            return memory::is_valid(input) ? input->body_angles() : vec2_t{};
        }

        inline void set_view_angles(sdk::BasePlayer* local, const vec2_t& angles) {
            if (!memory::is_valid(local)) return;
            auto* input = local->input();
            if (memory::is_valid(input))
                input->set_body_angles(angles);
        }

        inline float fov_to_px(float fov_deg, float half_h) {
            constexpr float DEG_TO_RAD_HALF = 0.00872664625f;
            constexpr float BASE = 90.f;
            return (std::tanf(fov_deg * DEG_TO_RAD_HALF) /
                std::tanf(BASE * DEG_TO_RAD_HALF)) * half_h;
        }

        inline std::vector<int> get_scan_bones() {
            std::vector<int> bones;
            const int mode = config::aimbot::selection_mode.value;

            switch (mode) {
            case 0: bones.push_back(enums::e_bone::head);   break;
            case 1: bones.push_back(enums::e_bone::neck);   break;
            case 2: bones.push_back(enums::e_bone::spine4); break;
            case 3: {
                const auto& list = config::aimbot::scan_bones.value;
                std::vector<int> pool;
                if (std::find(list.begin(), list.end(), 0) != list.end()) pool.push_back(enums::e_bone::head);
                if (std::find(list.begin(), list.end(), 1) != list.end()) pool.push_back(enums::e_bone::neck);
                if (std::find(list.begin(), list.end(), 2) != list.end()) pool.push_back(enums::e_bone::spine4);
                if (std::find(list.begin(), list.end(), 3) != list.end()) pool.push_back(enums::e_bone::spine3);
                if (std::find(list.begin(), list.end(), 4) != list.end()) pool.push_back(enums::e_bone::spine1);
                if (std::find(list.begin(), list.end(), 5) != list.end()) pool.push_back(enums::e_bone::pelvis);
                if (std::find(list.begin(), list.end(), 6) != list.end()) pool.push_back(enums::e_bone::l_hip);
                if (std::find(list.begin(), list.end(), 7) != list.end()) pool.push_back(enums::e_bone::r_hip);
                bones.push_back(pool.empty() ? (int)enums::e_bone::head : pool[rand() % pool.size()]);
                break;
            }
            case 4: {
                const auto& list = config::aimbot::scan_bones.value;
                if (!list.empty()) {
                    if (std::find(list.begin(), list.end(), 0) != list.end()) bones.push_back(enums::e_bone::head);
                    if (std::find(list.begin(), list.end(), 1) != list.end()) bones.push_back(enums::e_bone::neck);
                    if (std::find(list.begin(), list.end(), 2) != list.end()) bones.push_back(enums::e_bone::spine4);
                    if (std::find(list.begin(), list.end(), 3) != list.end()) bones.push_back(enums::e_bone::spine3);
                    if (std::find(list.begin(), list.end(), 4) != list.end()) bones.push_back(enums::e_bone::spine1);
                    if (std::find(list.begin(), list.end(), 5) != list.end()) bones.push_back(enums::e_bone::pelvis);
                    if (std::find(list.begin(), list.end(), 6) != list.end()) bones.push_back(enums::e_bone::l_hip);
                    if (std::find(list.begin(), list.end(), 7) != list.end()) bones.push_back(enums::e_bone::r_hip);
                }
                else {
                    bones = {
                        enums::e_bone::head,       enums::e_bone::spine1, enums::e_bone::spine2,
                        enums::e_bone::spine3,     enums::e_bone::spine4, enums::e_bone::pelvis,
                        enums::e_bone::neck,       enums::e_bone::l_hip,  enums::e_bone::r_hip,
                        enums::e_bone::l_knee,     enums::e_bone::r_knee,
                        enums::e_bone::l_upperarm, enums::e_bone::r_upperarm
                    };
                }
                break;
            }
            default: bones.push_back(enums::e_bone::head); break;
            }

            return bones;
        }

        inline bool is_target_valid(sdk::BasePlayer* player) {
            if (!memory::is_valid(player))   return false;
            if (player->lifestate() != 0)    return false;
            if (player == sdk::local_player) return false;

            const auto& filters = config::aimbot::filters.value;
            if (std::find(filters.begin(), filters.end(), 0) != filters.end() && player->is_teammate()) return false;
            if (std::find(filters.begin(), filters.end(), 1) != filters.end() && player->is_sleeping()) return false;

            auto* model = player->model();
            bool  is_npc = memory::is_valid(model) && model->is_npc();
            if (std::find(filters.begin(), filters.end(), 2) != filters.end() && is_npc)              return false;
            if (std::find(filters.begin(), filters.end(), 3) != filters.end() && player->is_knocked()) return false;

            return true;
        }

        inline aimbot_target_t find_best_target(const vec3_t& local_eyes, float fov_limit) {
            aimbot_target_t best{};
            best.fov = FLT_MAX;

            const vec2_t center = {
                static_cast<float>(unity::Screen::width()) * 0.5f,
                static_cast<float>(unity::Screen::height()) * 0.5f
            };

            const float            fov_px = fov_to_px(fov_limit, center.y);
            std::vector<sdk::BasePlayer*> players;
            {
                std::lock_guard<std::mutex> lock(sdk::info.mtx);
                players = sdk::info.players;
            }
            const std::vector<int> scan_bones = get_scan_bones();

            if (scan_bones.empty()) return best;

            for (auto* player : players) {
                if (!is_target_valid(player)) continue;

                const vec3_t vel = player->get_world_velocity();

                for (int bone_id : scan_bones) {
                    const vec3_t bone_pos = player->bone_position(static_cast<enums::e_bone>(bone_id));
                    if (bone_pos.empty()) continue;

                    const float dist = local_eyes.distance_to(bone_pos);
                    if (dist > config::aimbot::max_distance.value) continue;

                    vec2_t screen_pos;
                    auto   ctx = unity::Camera::get_frame_context();
                    if (!ctx.project(bone_pos, screen_pos)) continue;

                    float fov_metric = screen_pos.distance_to(center);
                    if (player == g_last_target && bone_id == g_last_bone) {
                        fov_metric *= 0.65f; // Apply stickiness discount to prevent jitter
                    }

                    if (fov_metric > fov_px || fov_metric >= best.fov) continue;

                    if (config::aimbot::visible_check.value)
                        if (!sdk::GamePhysics::is_visible(local_eyes, bone_pos)) continue;

                    best.player = player;
                    best.bone_id = bone_id;
                    best.position = bone_pos;
                    best.velocity = vel;
                    best.is_visible = true;
                    best.distance = dist;
                    best.fov = fov_metric;
                }
            }

            return best;
        }

        inline void run() {
            if (!config::aimbot::legit_enabled.value) return;

            auto* local = sdk::local_player;
            if (!memory::is_valid(local) || local->lifestate() || local->is_sleeping()) return;

            const ULONGLONG now = GetTickCount64();

            if (now - g_kill_time < static_cast<ULONGLONG>(config::aimbot::legit_kill_delay.value)) return;

            if (g_last_target) {
                std::vector<sdk::BasePlayer*> players_snap;
                {
                    std::lock_guard<std::mutex> lock(sdk::info.mtx);
                    players_snap = sdk::info.players;
                }
                bool found = false;
                for (auto* p : players_snap) {
                    if (p == g_last_target) { found = true; break; }
                }

                const bool target_dead = !found
                    || !memory::is_valid(g_last_target)
                    || g_last_target->lifestate() != 0;

                if (target_dead) {
                    g_kill_time = now;
                    g_last_target = nullptr;
                    g_last_bone = -1;
                    return;
                }
            }

            const int key = config::aimbot::legit_aim_key.value;
            const int kmode = config::aimbot::legit_aim_key_mode.value;

            bool active = false;
            if (key != 0) {
                switch (kmode) {
                case 0:  active = (GetAsyncKeyState(key) & 0x8000) != 0; break;
                case 1:  active = (GetKeyState(key) & 0x0001) != 0;      break;
                default: active = true; break;
                }
            }
            else {
                active = (kmode == 2);
            }

            if (!active) {
                g_last_recoil = {};
                g_rcs_initialized = false;
                g_last_target = nullptr;
                g_last_bone = -1;
                return;
            }

            const bool lbutton_down = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;

            if (lbutton_down && !g_was_lbutton_down) g_aim_start_time = now;
            if (!lbutton_down && g_was_lbutton_down) g_last_shot_time = now;
            g_was_lbutton_down = lbutton_down;

            if (lbutton_down &&
                now - g_aim_start_time < static_cast<ULONGLONG>(config::aimbot::legit_delay_before_shot.value)) return;

            if (now - g_last_shot_time < static_cast<ULONGLONG>(config::aimbot::legit_delay_after_shot.value)) return;

            auto* eyes = local->eyes();
            if (!memory::is_valid(eyes)) { LOG_ERROR("legitbot: eyes null"); return; }

            const vec3_t local_eyes = eyes->position();
            if (local_eyes.empty()) { LOG_ERROR("legitbot: eyes pos empty"); return; }

            bool is_aiming = false;
            auto* ms = local->model_state();
            if (memory::is_valid(ms))
                is_aiming = ms->has_flag(enums::e_model_state_flags::in_aim);

            const float base_fov = config::aimbot::fov.value * 1.1f;
            const float current_fov = (config::aimbot::dynamic_fov.value && is_aiming)
                ? base_fov * 1.4f
                : base_fov;

            // Target acquisition
            const aimbot_target_t target = find_best_target(local_eyes, current_fov);
            bool has_target = (target.player != nullptr);

            if (has_target) {
                if (target.player != g_last_target) {
                    g_target_acquire_time = now;
                    g_last_target = target.player;
                }
                g_last_bone = target.bone_id;
            } else {
                g_last_target = nullptr;
                g_last_bone = -1;
            }

            vec2_t current_angles = get_view_angles(local);
            vec2_t final_angles = current_angles;

            if (has_target) {
                // ── 1. Target is valid: run legit aimbot ─────────────────────────────
                if (now - g_target_acquire_time >= static_cast<ULONGLONG>(config::aimbot::legit_reaction_delay.value)) {
                    
                    // Position smoothing
                    if (g_pos_last_target != target.player) {
                        g_pos_history[0] = g_pos_history[1] = g_pos_history[2] = target.position;
                        g_pos_idx = 0;
                    }
                    g_pos_last_target = target.player;
                    g_pos_history[g_pos_idx % 3] = target.position;
                    g_pos_idx++;

                    vec3_t smoothed_pos{};
                    for (const auto& p : g_pos_history) smoothed_pos += p;
                    smoothed_pos = smoothed_pos / 3.f;

                    vec3_t aim_pos = smoothed_pos;

                    const auto pred = prediction::c_prediction::run(
                        local_eyes, smoothed_pos, target.velocity,
                        0.2f, -1.f, 8.f, -1.f, -1.f);

                    if (pred.possible)
                        aim_pos = pred.position;

                    vec2_t aim_angles = calc_angle(local_eyes, aim_pos);

                    // Advanced Recoil Compensation (Direct active compensation)
                    if (config::aimbot::legit_rcs.value) {
                        auto* input = local->input();
                        if (memory::is_valid(input)) {
                            const vec2_t recoil = input->recoil_angles();
                            const float  rcs_amount = config::aimbot::legit_rcs_amount.value / 100.f;

                            aim_angles.x -= recoil.x * rcs_amount;
                            aim_angles.y -= recoil.y * rcs_amount;

                            normalize_angles(aim_angles.x, aim_angles.y);
                        }
                    }

                    if (config::aimbot::legit_smooth.value > 1.f)
                        apply_smooth(current_angles, aim_angles, config::aimbot::legit_smooth.value);
                    else
                        current_angles = aim_angles;

                    final_angles = current_angles;
                    g_rcs_initialized = false; // Reset standalone RCS so it doesn't conflict
                }
            }
            else {
                // ── 2. No target acquired: run Standalone RCS (when shooting) ────────
                if (config::aimbot::legit_rcs.value && lbutton_down) {
                    auto* input = local->input();
                    if (memory::is_valid(input)) {
                        const vec2_t recoil = input->recoil_angles();
                        const float  rcs_amount = config::aimbot::legit_rcs_amount.value / 100.f;

                        if (!g_rcs_initialized) {
                            g_last_recoil = recoil;
                            g_rcs_initialized = true;
                        }

                        const vec2_t delta = { recoil.x - g_last_recoil.x, recoil.y - g_last_recoil.y };

                        if (std::abs(delta.x) > 0.01f || std::abs(delta.y) > 0.01f) {
                            final_angles.x -= delta.x * rcs_amount;
                            final_angles.y -= delta.y * rcs_amount;
                            normalize_angles(final_angles.x, final_angles.y);
                        }

                        g_last_recoil = recoil;
                    }
                }
                else {
                    g_rcs_initialized = false;
                }
            }

            // Write modified angles back to the game
            if (has_target || (config::aimbot::legit_rcs.value && g_rcs_initialized)) {
                set_view_angles(local, final_angles);
            }
        }

    } // namespace legitbot
} // namespace aimbot