#pragma once
#include "simulation/simulation.h"
#include <cmath>

namespace prediction {

    static inline vec3_t safe_normalize(const vec3_t& v) {
        float len = v.length();
        if (len < 0.0001f) return vec3_t(0.f, 0.f, 0.f);
        return v / len;
    }

    static inline bool is_finite_vec(const vec3_t& v) {
        return std::isfinite(v.x) && std::isfinite(v.y) && std::isfinite(v.z);
    }

    struct predict_params_t {
        vec3_t eye_pos{};
        vec3_t target_pos{};
        float  speed{ 0.f };
        float  drag{ 0.f };
        float  gravity_modifier{ 0.f };
    };

    struct predict_result_t {
        vec3_t aim_point{};
        vec3_t velocity{};
        bool   valid{ false };
    };

    class c_prediction {
    public:
        static inline c_prediction& get() {
            static c_prediction instance;
            return instance;
        }

        c_prediction(const c_prediction&) = delete;
        c_prediction& operator=(const c_prediction&) = delete;

        static inline prediction_data_t run(
            vec3_t start_position,
            vec3_t target_position,
            vec3_t target_velocity,
            float  prediction_accuracy = 0.2f,
            float  bullet_speed_override = 0.f,
            float  max_travel_time = 8.f,
            float  drag_override = -1.f,
            float  gravity_override = -1.f)
        {
            prediction_data_t result;
            result.possible = false;
            result.position = target_position;

            float  drag = 0.f;
            float  bullet_speed = 0.f;
            vec3_t gravity = vec3_t(0.f, -9.81f, 0.f);

            bool has_data = c_projectile_simulation::get_projectile_data(&drag, &gravity, &bullet_speed, nullptr);

            if (!has_data && bullet_speed_override <= 0.f)
                return result;

            if (drag_override >= 0.f)        drag = drag_override;
            if (gravity_override >= 0.f)     gravity = vec3_t(0.f, -9.81f * gravity_override, 0.f);
            if (bullet_speed_override > 0.f) bullet_speed = bullet_speed_override;

            if (!std::isfinite(bullet_speed) || bullet_speed < 1.f) return result;
            if (!std::isfinite(drag))    drag = 0.f;
            if (!is_finite_vec(gravity)) gravity = vec3_t(0.f, -9.81f, 0.f);
            if (!is_finite_vec(start_position) || !is_finite_vec(target_position)) return result;

            const float dist_to_target = start_position.distance_to(target_position);

            if (dist_to_target < 0.5f) {
                result.possible = true;
                result.position = target_position;
                return result;
            }

            vec3_t aim_point = target_position;
            {
                float travel_estimate = dist_to_target / bullet_speed;
                aim_point -= gravity * (0.5f * travel_estimate * travel_estimate);
                if (target_velocity.length() > 0.01f)
                    aim_point += target_velocity * travel_estimate;
            }

            for (int iter = 0; iter < 20; iter++) {
                if (!is_finite_vec(aim_point)) { aim_point = target_position; break; }

                vec3_t aim_dir = safe_normalize(aim_point - start_position);
                if (aim_dir.length() < 0.0001f) break;

                vec3_t sim_vel = aim_dir * bullet_speed;
                vec3_t sim_pos = start_position;
                float  best_dist = FLT_MAX;
                vec3_t best_point = aim_point;
                float  sim_time = 0.f;

                for (int step = 0; step < 256; step++) {
                    const vec3_t prev_pos = sim_pos;

                    sim_pos += sim_vel * 0.03125f;
                    sim_vel += gravity * 0.03125f;
                    sim_vel -= sim_vel * (drag * 0.03125f);
                    sim_time += 0.03125f;

                    if (!is_finite_vec(sim_pos)) break;

                    const vec3_t predicted_target = target_position + target_velocity * sim_time;
                    const vec3_t seg_dir = sim_pos - prev_pos;
                    const float  seg_len = seg_dir.length();
                    vec3_t       nearest;

                    if (seg_len < 0.0001f) {
                        nearest = prev_pos;
                    }
                    else {
                        const vec3_t seg_norm = seg_dir / seg_len;
                        float proj = (predicted_target - prev_pos).dot(seg_norm);
                        if (!(proj > 0.f))     proj = 0.f;
                        if (!(proj < seg_len)) proj = seg_len;
                        nearest = prev_pos + seg_norm * proj;
                    }

                    const float current_dist = nearest.distance_to(predicted_target);
                    if (!std::isfinite(current_dist)) break;

                    if (current_dist < best_dist) {
                        best_dist = current_dist;
                        best_point = nearest;
                    }

                    if (sim_pos.distance_to(start_position) > dist_to_target * 1.5f + 30.f) break;
                    if (sim_pos.y < start_position.y - 150.f) break;
                }

                if (best_dist < 0.05f) break;

                const vec3_t impact_target = target_position + target_velocity * sim_time;
                vec3_t       error = impact_target - best_point;

                if (!is_finite_vec(error)) break;

                const float err_len = error.length();
                if (err_len > 15.f) error = (error / err_len) * 15.f;

                const float lerp_factor = (iter > 5) ? 0.5f : 1.0f;
                aim_point += error * lerp_factor;
            }

            if (!is_finite_vec(aim_point)) return result;

            result.possible = true;
            result.position = aim_point;
            return result;
        }

        static inline vec3_t get_aim_direction(
            vec3_t start_position,
            vec3_t target_position,
            vec3_t target_velocity,
            float  original_speed,
            float  initial_distance)
        {
            float  drag = 0.f;
            vec3_t gravity = vec3_t(0.f, -9.81f, 0.f);

            c_projectile_simulation::get_projectile_data(&drag, &gravity, nullptr, nullptr);

            if (!std::isfinite(drag))                                    drag = 0.f;
            if (!is_finite_vec(gravity))                                 gravity = vec3_t(0.f, -9.81f, 0.f);
            if (!std::isfinite(original_speed) || original_speed < 1.f) return target_position;
            if (!is_finite_vec(start_position) || !is_finite_vec(target_position)) return target_position;

            vec3_t temporary_target = target_position;

            constexpr int   max_iter = 50;
            constexpr float timestep = 0.03125f;
            const     int   max_steps = static_cast<int>(8.0f / timestep);

            for (int iter = 0; iter < max_iter; iter++) {
                if (!is_finite_vec(temporary_target)) { temporary_target = target_position; break; }

                vec3_t aim_dir = safe_normalize(temporary_target - start_position);
                if (aim_dir.length() < 0.0001f) break;

                vec3_t sim_vel = aim_dir * original_speed;
                vec3_t cur_pos = start_position;
                vec3_t prev_pos = cur_pos;
                vec3_t missed_offset{};
                float  closest_dist = FLT_MAX;
                vec3_t sim_target = target_position;
                float  sim_time = 0.f;

                for (int step = 0; step < max_steps; step++) {
                    prev_pos = cur_pos;
                    cur_pos += sim_vel * timestep;
                    sim_vel += gravity * timestep;
                    sim_vel -= sim_vel * (drag * timestep);

                    if (!is_finite_vec(cur_pos)) break;

                    const float traveled = start_position.distance_to(cur_pos);
                    if (initial_distance == 0.f || traveled > initial_distance) {
                        sim_time += timestep;
                        sim_target = target_position + target_velocity * sim_time;
                    }
                    else {
                        sim_target = target_position;
                    }

                    const vec3_t seg_dir = cur_pos - prev_pos;
                    const float  seg_len = seg_dir.length();
                    vec3_t       nearest;

                    if (seg_len < 0.0001f) {
                        nearest = prev_pos;
                    }
                    else {
                        const vec3_t seg_norm = seg_dir / seg_len;
                        float proj = (sim_target - prev_pos).dot(seg_norm);
                        if (!(proj > 0.f))     proj = 0.f;
                        if (!(proj < seg_len)) proj = seg_len;
                        nearest = prev_pos + seg_norm * proj;
                    }

                    const float dist_missed = nearest.distance_to(sim_target);
                    if (!std::isfinite(dist_missed)) break;

                    if (dist_missed < 0.1f) return temporary_target;

                    if (dist_missed < closest_dist) {
                        closest_dist = dist_missed;
                        missed_offset = sim_target - nearest;
                    }
                }

                if (!is_finite_vec(missed_offset)) break;
                temporary_target += missed_offset;
            }

            return is_finite_vec(temporary_target) ? temporary_target : target_position;
        }

        static inline predict_result_t predict(
            sdk::BasePlayer* target,
            const predict_params_t& params)
        {
            predict_result_t result{};

            if (!memory::is_valid(target)) {
                LOG_ERROR("predict: target invalid");
                return result;
            }
            if (params.speed <= 0.f) {
                LOG_ERROR("predict: speed <= 0");
                return result;
            }
            if (!is_finite_vec(params.eye_pos) || !is_finite_vec(params.target_pos)) {
                LOG_ERROR("predict: invalid positions");
                return result;
            }

            vec3_t gravity = unity::Physics::gravity();
            if (!is_finite_vec(gravity)) gravity = vec3_t(0.f, -9.81f, 0.f);

            simulation_params_t sim_params{};
            sim_params.speed = params.speed;
            sim_params.drag = params.drag;
            sim_params.gravity_modifier = params.gravity_modifier;
            sim_params.gravity = gravity;

            auto phase1 = c_projectile_simulation::simulate(params.eye_pos, params.target_pos, sim_params);

            if (!phase1.hit) return result;
            if (!std::isfinite(phase1.bullet_time) || phase1.bullet_time <= 0.f) return result;

            const bool   mounted = memory::is_valid(target->mounted());
            const vec3_t wv = target->get_world_velocity();
            vec3_t       player_velocity = mounted ? wv : vec3_t(wv.x, 0.f, wv.z);

            const vec3_t corrected_target = params.target_pos + player_velocity * (0.75f * phase1.bullet_time);
            if (!is_finite_vec(corrected_target)) return result;

            auto phase2 = c_projectile_simulation::simulate(params.eye_pos, corrected_target, sim_params);

            if (!phase2.hit || !is_finite_vec(phase2.aim_point)) return result;

            result.aim_point = phase2.aim_point;
            result.velocity = safe_normalize(phase2.aim_point - params.eye_pos) * params.speed;
            result.valid = true;
            return result;
        }

    private:
        c_prediction() = default;
        ~c_prediction() = default;
    };

} // namespace prediction