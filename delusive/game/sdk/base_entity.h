#pragma once
#include "../il2cpp/il2cpp.h"
#include "base_networkable.h"
#include "math/vector.h"
#include "math/bounds.h"
#include "model.h"
#include "system/string.h"
#include "enums/enums.h"
#include <string>

namespace sdk {
	extern vec3_t local_position;

	class BaseEntity : public BaseNetworkable {
	private:
		static inline uintptr_t bounds_ptr = 0;
		static inline uintptr_t entity_model_ptr = 0;
		static inline uintptr_t hit_direction_ptr = 0;

		static inline uintptr_t server_rpc_1_ptr = 0;
		static inline uintptr_t server_rpc_4_ptr = 0;
		static inline uintptr_t get_parent_velocity_ptr = 0;
		static inline uintptr_t send_signal_broadcast_ptr = 0;
		static inline uintptr_t max_velocity_ptr = 0;
		static inline uintptr_t bounds_padding_ptr = 0;

	public:
		static void initialize() {
			auto klass = il2cpp::get_class(_(""), _("BaseEntity"));

			il2cpp_field_offset(bounds_ptr, _(""), _("BaseEntity"), _("bounds"));
			il2cpp_field_offset(entity_model_ptr, _(""), _("BaseEntity"), _("model"));
			il2cpp_field_offset(hit_direction_ptr, _(""), _("TreeEntity"), _("hitDirection"));

			server_rpc_1_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("ServerRPC"), 1));
			server_rpc_4_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("ServerRPC"), 4));
			max_velocity_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("MaxVelocity"), -1));
			bounds_padding_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("BoundsPadding"), -1));
			get_parent_velocity_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("GetParentVelocity"), -1));
			send_signal_broadcast_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("SendSignalBroadcast"), 2));
		}

		il2cpp_field_get(bounds_t, bounds, bounds_ptr);
		il2cpp_field_get(Model*, entity_model, entity_model_ptr);
		il2cpp_field_set(vec3_t, set_hit_direction, hit_direction_ptr);

		inline void server_rpc(const char* name) {
			safe.call<void>(server_rpc_1_ptr, this, il2cpp::new_string(name));
		}

		inline void server_rpc(const char* rpc, const char* string, bool value) {
			safe.call<void>(server_rpc_4_ptr, this, il2cpp::new_string(rpc), il2cpp::new_string(string), value);
		}

		inline vec3_t parent_velocity() {
			return safe.call<vec3_t>(get_parent_velocity_ptr, this);
		}

		inline void send_signal_broadcast(enums::e_signal signal, const char* rpc = "") {
			safe.call<void>(send_signal_broadcast_ptr, this, signal, il2cpp::new_string(rpc));
		}

		inline float max_velocity() {
			return safe.call<float>(max_velocity_ptr, this);
		}

		inline float bounds_padding() {
			return safe.call<float>(bounds_padding_ptr, this);
		}

		template<typename T = void*>
		static T find_closest(std::string name, float max_distance) {
			T best_entity = nullptr;
			float best_distance = max_distance;

			const auto& client_entities_obj = BaseNetworkable::client_entities();
			if (!memory::is_valid(client_entities_obj))
				return T();

			const auto& entity_list_obj = client_entities_obj->entity_list();
			if (!memory::is_valid(entity_list_obj))
				return T();

			const auto& count = entity_list_obj->count();
			if (count <= 0)
				return T();

			for (auto i = 0; i < count; ++i) {
				const auto& base_networkable = entity_list_obj->get<BaseNetworkable*>(i);
				if (!memory::is_valid(base_networkable))
					continue;

				if (!base_networkable->is_class(name.c_str()))
					continue;

				const auto& transform = base_networkable->transform();
				if (!memory::is_valid(transform))
					continue;

				const auto& distance = local_position.distance_to(transform->position());
				if (distance < best_distance) {
					best_distance = distance;
					best_entity = reinterpret_cast<T>(base_networkable);
				}
			}

			return best_entity;
		}
	};
}
