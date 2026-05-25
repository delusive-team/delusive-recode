#pragma once
#include "../../il2cpp/il2cpp.h"

namespace protobuf {
	class Environment {
	private:
		static inline uintptr_t date_time_ptr = 0;

	public:
		static void initialize() {
			il2cpp_field_offset(date_time_ptr, _("ProtoBuf"), _("Environment"), _("dateTime"));
		}

		il2cpp_field_get(long long, date_time, date_time_ptr);
	};
}
