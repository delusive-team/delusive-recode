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
		static inline uintptr_t get_culling_mask_ptr = 0;
		static inline uintptr_t set_culling_mask_ptr = 0;

	public:
		static void initialize() {
			auto klass = il2cpp::get_class(_("UnityEngine"), _("Camera"));
			set_field_of_view_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("set_fieldOfView"), 1));
			get_field_of_view_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("get_fieldOfView"), -1));
			get_culling_mask_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("get_cullingMask"), -1));
			set_culling_mask_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("set_cullingMask"), 1));
			
			set_aspect_ptr = il2cpp::resolve_icall(_("UnityEngine.Camera::set_aspect(System.Single)"));
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

		inline bool world_to_screen_point(vec3_t position, vec2_t& screen_point) {
			const auto& object = mem::read<uintptr_t>((uintptr_t)this + 0x10);
			if (!memory::is_valid(object))
				return false;

			auto matrix = mem::read<mat4x4_t>(object + 0x2E4);

			__m128 trans_vec = _mm_set_ps(0.0f, matrix.m[2][3], matrix.m[1][3], matrix.m[0][3]);
			__m128 right_vec = _mm_set_ps(0.0f, matrix.m[2][0], matrix.m[1][0], matrix.m[0][0]);
			__m128 up_vec = _mm_set_ps(0.0f, matrix.m[2][1], matrix.m[1][1], matrix.m[0][1]);

			__m128 element_pos = _mm_set_ps(0.0f, position.z, position.y, position.x);

			__m128 dot_trans = _mm_dp_ps(trans_vec, element_pos, 0x71);
			__m128 dot_right = _mm_dp_ps(right_vec, element_pos, 0x71);
			__m128 dot_up = _mm_dp_ps(up_vec, element_pos, 0x71);

			float result_trans[4];
			float result_right[4];
			float result_up[4];

			_mm_store_ps(result_trans, dot_trans);
			_mm_store_ps(result_right, dot_right);
			_mm_store_ps(result_up, dot_up);

			float w = result_trans[0] + matrix.m[3][3];

			if (w < 0.001f)
				return false;

			float x = result_right[0] + matrix.m[3][0];
			float y = result_up[0] + matrix.m[3][1];

			screen_point = { std::floorf((Screen::width() / 2.f) * (1.f + x / w)), std::floorf((Screen::height() / 2.f) * (1.f - y / w)) };

			return true;
		}

		inline float calculate_fov(vec3_t pos) {
			vec2_t screen_point;
			if (!world_to_screen_point(pos, screen_point))
				return 10000.f;

			const auto& get_2d_dist = [&](const vec2_t& source, const vec2_t& dest) {
				return sqrtf((source.x - dest.x) * (source.x - dest.x) + (source.y - dest.y) * (source.y - dest.y));
			};

			return get_2d_dist(vec2_t(Screen::width() / 2.f, Screen::height() / 2.f), screen_point);
		}

		inline bool is_in_fov(const vec3_t& pos, float fov_threshold) {
			float fov = calculate_fov(pos);
			return fov <= fov_threshold;
		}
	};
}
