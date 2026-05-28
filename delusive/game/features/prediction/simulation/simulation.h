#pragma once
#include "../../../sdk/sdk.h"
#include <algorithm>
#include <cmath>

namespace prediction {

    struct line_t {
        vec3_t start{};
        vec3_t end{};

        line_t() = default;
        line_t(const vec3_t& start_pos, const vec3_t& end_pos) : start(start_pos), end(end_pos) {}

        inline vec3_t closest_point(const vec3_t& pos) const {
            vec3_t direction = end - start;
            float  magnitude = direction.length();
            if (magnitude < 0.0001f) return start;
            vec3_t norm_dir = direction / magnitude;
            float  projection = (pos - start).dot(norm_dir);
            if (!(projection > 0.f))       projection = 0.f;
            if (!(projection < magnitude)) projection = magnitude;
            return start + norm_dir * projection;
        }
    };

    struct trace_result_t {
        bool did_hit{ false };
        bool silent_cat{ false };
        bool can_hit{ false };

        sdk::BasePlayer* hit_entity{ nullptr };

        vec3_t hit_position{};
        vec3_t hit_velocity{};
        vec3_t player_position{};
        vec3_t out_velocity{};

        float hit_time{ 0.f };
        float desired_time{ 0.f };
        float hit_dist{ FLT_MAX };
    };

    struct prediction_data_t {
        bool possible{ false };
        int  difficulty{ 0 };

        vec3_t position{};
        vec3_t out_velocity{};
        vec3_t hit_velocity{};
        vec3_t gravity{};
        vec3_t angle{};

        float hit_time{ 0.f };
        float hit_mismatch{ 0.f };
        float drag{ 0.f };
    };

    struct simulation_params_t {
        float  speed{ 0.f };
        float  drag{ 0.f };
        float  gravity_modifier{ 0.f };
        vec3_t gravity{};
        float  timestep{ 0.03125f };
        int    max_steps{ 256 };
        int    max_iterations{ 30 };
        float  hit_radius{ 0.2f };
    };

    struct simulation_result_t {
        vec3_t aim_point{};
        float  bullet_time{ 0.f };
        bool   hit{ false };
    };

    class c_projectile_simulation {
    private:
        static inline uintptr_t projectile_type = 0;
        static inline uintptr_t item_mod_projectile_type = 0;

    public:
        static inline void reset_cache() {
            projectile_type = 0;
            item_mod_projectile_type = 0;
        }

        static inline bool get_projectile_data(float* out_drag, vec3_t* out_gravity, float* out_bullet_speed, float* out_penetration) {
            auto* local = sdk::local_player;
            if (!memory::is_valid(local)) return false;

            auto* item = local->item();
            if (!memory::is_valid(item)) return false;

            auto* held_entity = item->held_entity();
            if (!memory::is_valid(held_entity)) return false;

            if (!projectile_type)
                projectile_type = il2cpp::type_object(_(""), _("Projectile"));
            if (!item_mod_projectile_type)
                item_mod_projectile_type = il2cpp::type_object(_(""), _("ItemModProjectile"));

            if (!memory::is_valid(projectile_type) || !memory::is_valid(item_mod_projectile_type))
                return false;

            sdk::ItemModProjectile* item_mod_projectile = nullptr;

            if (item->is_weapon()) {
                auto* magazine = held_entity->primary_magazine();
                if (!memory::is_valid(magazine)) return false;

                auto* ammo_type = magazine->ammo_type();
                if (!memory::is_valid(ammo_type)) return false;

                item_mod_projectile = ammo_type->component<sdk::ItemModProjectile*>(item_mod_projectile_type);
            }
            else {
                auto* info = item->info();
                if (!memory::is_valid(info)) return false;

                item_mod_projectile = info->component<sdk::ItemModProjectile*>(item_mod_projectile_type);
            }

            if (!memory::is_valid(item_mod_projectile)) return false;

            auto* projectile_object_ref = item_mod_projectile->projectile_object();
            if (!memory::is_valid(projectile_object_ref)) return false;

            auto* guid = projectile_object_ref->guid();
            if (!memory::is_valid(guid)) return false;

            auto* projectile_object = sdk::GameManifest::guid_to_object(guid);
            if (!memory::is_valid(projectile_object)) return false;

            auto* projectile = projectile_object->component<sdk::Projectile*>(projectile_type);
            if (!memory::is_valid(projectile)) return false;

            if (out_bullet_speed) {
                float  modifier = item->is_weapon() ? held_entity->projectile_velocity_scale(false) : 1.f;
                vec3_t initial_vel = projectile->initial_velocity();
                float  base_speed = initial_vel.length();

                if (base_speed < 0.1f) {
                    base_speed = item_mod_projectile->projectile_velocity()
                        + item_mod_projectile->projectile_velocity_spread();
                }

                *out_bullet_speed = base_speed * modifier;
            }

            if (out_drag)        *out_drag = projectile->drag();
            if (out_gravity)     *out_gravity = vec3_t(0.f, -9.81f * projectile->gravity_modifier(), 0.f);
            if (out_penetration) *out_penetration = projectile->penetration_power();

            return true;
        }

        static inline trace_result_t trace_projectile(vec3_t position, vec3_t velocity, float drag, vec3_t gravity, vec3_t target_point, bool draw = false, float max_travel_time = 8.f) {
            constexpr float time_step = 0.03125f;

            vec3_t prev_position = position;
            float  prev_dist = FLT_MAX;
            float  travel_time = 0.f;

            line_t         result_line(position, position);
            trace_result_t result;
            result.out_velocity = velocity;

            vec3_t hit_velocity = velocity;
            if (max_travel_time <= 0.f) max_travel_time = 8.f;

            while (travel_time < max_travel_time) {
                prev_position = position;
                position += velocity * time_step;

                line_t line(prev_position, position);
                vec3_t nearest = line.closest_point(target_point);

                float dst = nearest.distance_to(target_point);
                if (dst > prev_dist) break;

                prev_dist = dst;
                result_line = line;

                velocity += gravity * time_step;
                velocity -= velocity * drag * time_step;

                travel_time += time_step;
                hit_velocity = velocity;
            }

            vec3_t hit_pos = result_line.closest_point(target_point);

            result.hit_dist = hit_pos.distance_to(target_point);
            result.hit_position = hit_pos;
            result.hit_velocity = hit_velocity;
            result.hit_time = travel_time - time_step;
            result.did_hit = true;

            return result;
        }

        static inline simulation_result_t simulate(const vec3_t& origin, const vec3_t& target, const simulation_params_t& params) {
            simulation_result_t result{};
            result.aim_point = target;

            vec3_t target_position = target;

            for (int sim = 0; sim < params.max_iterations; sim++) {
                vec3_t aim_dir = (target_position - origin).normalized();
                if (aim_dir.empty()) break;

                vec3_t vel = aim_dir * params.speed;
                vec3_t current_position = origin;
                vec3_t previous_position = current_position;
                float  closest_dist = FLT_MAX;
                vec3_t closest_point{};
                vec3_t offset{};
                bool   hit_player = false;

                for (int i = 0; i < params.max_steps; i++) {
                    previous_position = current_position;
                    current_position += vel * params.timestep;
                    vel += params.gravity * params.gravity_modifier * params.timestep;
                    vel -= vel * (params.drag * params.timestep);

                    vec3_t line = current_position - previous_position;
                    vec3_t v = target - previous_position;
                    float  line_sqr_len = line.dot(line);
                    float  d = (line_sqr_len > 0.f) ? (line.dot(v) / line_sqr_len) : 0.f;
                    d = std::clamp(d, 0.f, 1.f);

                    vec3_t nearest_point = previous_position + line * d;
                    float  dist_to_target = nearest_point.distance_to(target);

                    if (dist_to_target < params.hit_radius) {
                        result.bullet_time = (i + 1) * params.timestep;
                        hit_player = true;
                        break;
                    }

                    if (dist_to_target < closest_dist) {
                        closest_dist = dist_to_target;
                        closest_point = nearest_point;
                        offset = target - nearest_point;
                    }
                }

                if (hit_player) {
                    result.aim_point = target_position;
                    result.hit = true;
                    return result;
                }

                target_position += offset;
            }

            result.hit = false;
            return result;
        }
    };
}