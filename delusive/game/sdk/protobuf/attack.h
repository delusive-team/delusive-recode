#pragma once
#include "../../il2cpp/il2cpp.h"
#include "../math/vector.h"

namespace protobuf {
	class Attack {
	private:
		static inline uintptr_t point_end_ptr = 0;
		static inline uintptr_t hit_bone_ptr = 0;
		static inline uintptr_t hit_part_id_ptr = 0;
		static inline uintptr_t hit_normal_local_ptr = 0;
		static inline uintptr_t hit_position_local_ptr = 0;
	public:
		static void initialize() {
			il2cpp_field_offset(point_end_ptr, _("ProtoBuf"), _("Attack"), _("pointEnd"));
			il2cpp_field_offset(hit_bone_ptr, _("ProtoBuf"), _("Attack"), _("hitBone"));
			il2cpp_field_offset(hit_part_id_ptr, _("ProtoBuf"), _("Attack"), _("hitPartID"));
			il2cpp_field_offset(hit_normal_local_ptr, _("ProtoBuf"), _("Attack"), _("hitNormalLocal"));
			il2cpp_field_offset(hit_position_local_ptr, _("ProtoBuf"), _("Attack"), _("hitPositionLocal"));
		}

		il2cpp_field_get(vec3_t, point_end, point_end_ptr);

		il2cpp_field_set(unsigned int, set_hit_bone, hit_bone_ptr);
		il2cpp_field_set(unsigned int, set_hit_part_id, hit_part_id_ptr);
		il2cpp_field_set(vec3_t, set_hit_normal_local, hit_normal_local_ptr);
		il2cpp_field_set(vec3_t, set_hit_position_local, hit_position_local_ptr);
	};
}
