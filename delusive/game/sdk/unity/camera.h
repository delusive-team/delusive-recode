#pragma once
#include "../../il2cpp/il2cpp.h"
#include "component.h"
#include "screen.h"
#include "../math/vector.h"
#include "../math/matrix.h"
#include <immintrin.h>
#include <cmath>

namespace unity {
	class Camera : public Component {
	private:
		static inline uintptr_t set_field_of_view_ptr = 0;
		static inline uintptr_t get_field_of_view_ptr = 0;
		static inline uintptr_t set_aspect_ptr = 0;
		static inline uintptr_t set_main_ptr = 0;
		static inline uintptr_t get_culling_mask_ptr = 0;
		static inline uintptr_t set_culling_mask_ptr = 0;
		static inline uintptr_t cached_camera_ptr = 0;

	public:
		static void initialize() {
			auto klass = il2cpp::get_class(_("UnityEngine"), _("Camera"));
			set_field_of_view_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("set_fieldOfView"), 1));
			get_field_of_view_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("get_fieldOfView"), -1));
			get_culling_mask_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("get_cullingMask"), -1));
			set_culling_mask_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("set_cullingMask"), 1));
			
			set_aspect_ptr = il2cpp::resolve_icall(_("UnityEngine.Camera::set_aspect(System.Single)"));
			set_main_ptr = il2cpp::resolve_icall(_("UnityEngine.Camera::get_main()"));
		}

		inline int culling_mask() {
			return safe.call<int>(get_culling_mask_ptr, this);
		}

		inline void set_culling_mask(int mask) {
			safe.call<void>(set_culling_mask_ptr, this, mask);
		}

		inline float field_of_view() {
			return safe.call<float>(get_field_of_view_ptr, this);
		}

		inline void set_field_of_view(float fov) {
			safe.call<void>(set_field_of_view_ptr, this, fov);
		}

		inline void set_aspect(float aspect) {
			safe.call<void>(set_aspect_ptr, this, aspect);
		}

		static mat4x4_t view_matrix() {
			if (!cached_camera_ptr) cached_camera_ptr = safe.call<uintptr_t>(set_main_ptr);
			if (!memory::is_valid(cached_camera_ptr)) {
				cached_camera_ptr = 0;
				return mat4x4_t();
			}

			auto camera_native = *reinterpret_cast<uintptr_t*>(cached_camera_ptr + 0x10);
			if (!memory::is_valid(camera_native)) return mat4x4_t();

			auto matrix = *reinterpret_cast<mat4x4_t*>(camera_native + 0x2e4);

			if (matrix.m[0][0] == 0 && matrix.m[1][1] == 0 && matrix.m[2][2] == 0 && matrix.m[3][3] == 0) return mat4x4_t();

			return matrix;
		}

		struct FrameCtx {
			vec3_t right{}, up{}, trans{};
			float  x_bias = 0.f, y_bias = 0.f, w_bias = 0.f;
			float  half_w = 0.f, half_h = 0.f;

			bool project(const vec3_t& world, vec2_t& out) const {
				const float w = trans.dot(world) + w_bias;
				if (w < 0.001f) return false;

				const float inv_w = 1.f / w;
				const float x = (right.dot(world) + x_bias) * inv_w;
				const float y = (up.dot(world) + y_bias) * inv_w;

				out.x = half_w * (1.f + x);
				out.y = half_h * (1.f - y);
				return true;
			}
		};

		static FrameCtx get_frame_context() {
			FrameCtx ctx;
			auto matrix = view_matrix();
			if (matrix.m[0][0] == 0 && matrix.m[3][3] == 0) return ctx;

			const auto temp = matrix.transpose();
			ctx.right = { temp.m[0][0], temp.m[0][1], temp.m[0][2] };
			ctx.up = { temp.m[1][0], temp.m[1][1], temp.m[1][2] };
			ctx.trans = { temp.m[3][0], temp.m[3][1], temp.m[3][2] };
			ctx.x_bias = temp.m[0][3];
			ctx.y_bias = temp.m[1][3];
			ctx.w_bias = temp.m[3][3];

			ctx.half_w = static_cast<float>(Screen::width()) / 2.f;
			ctx.half_h = static_cast<float>(Screen::height()) / 2.f;
			return ctx;
		}

		static vec3_t world_to_screen(vec3_t wrld) {
			auto matrix = view_matrix();

			if (matrix.m[0][0] == 0 && matrix.m[3][3] == 0) {
				return vec3_t(0, 0, 0);
			}

			vec3_t out;
			const auto temp = matrix.transpose();

			vec3_t trans(temp.m[3][0], temp.m[3][1], temp.m[3][2]);
			vec3_t up(temp.m[1][0], temp.m[1][1], temp.m[1][2]);
			vec3_t right(temp.m[0][0], temp.m[0][1], temp.m[0][2]);

			float w = trans.dot(wrld) + temp.m[3][3];

			if (w < 0.001f) {
				return vec3_t(0, 0, 0);
			}

			float x = right.dot(wrld) + temp.m[0][3];
			float y = up.dot(wrld) + temp.m[1][3];

			out.x = (static_cast<float>(Screen::width()) / 2.f) * (1.f + x / w);
			out.y = (static_cast<float>(Screen::height()) / 2.f) * (1.f - y / w);
			out.z = 0.0f;

			return out;
		}

		static float calculate_fov(vec3_t pos) {
			auto screen_pos = world_to_screen(pos);
			if (screen_pos.empty())
				return 10000.f;

			auto get_2d_dist = [&](const vec2_t& source, const vec3_t& dest) {
				return std::sqrt(std::pow(source.x - dest.x, 2) + std::pow(source.y - dest.y, 2));
				};

			vec2_t center((float)Screen::width() / 2.f, (float)Screen::height() / 2.f);
			return get_2d_dist(center, screen_pos);
		}

		inline bool is_in_fov(const vec3_t& pos, float fov_threshold) {
			float fov = calculate_fov(pos);
			return fov <= fov_threshold;
		}
	};
}
