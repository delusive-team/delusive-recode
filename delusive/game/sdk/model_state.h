#pragma once
#include "../il2cpp/il2cpp.h"

namespace sdk {
	class ModelState {
	private:
		static inline uintptr_t flags_ptr = 0;
		static inline uintptr_t get_ducked_ptr = 0;
		static inline uintptr_t get_mounted_ptr = 0;
		static inline uintptr_t get_crawling_ptr = 0;
		static inline uintptr_t set_on_ladder_ptr = 0;
		static inline uintptr_t set_sprinting_ptr = 0;

	public:
		static void initialize() {
			il2cpp_field_offset(flags_ptr, _(""), _("ModelState"), _("flags"));
			
			get_ducked_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_(""), _("ModelState")), _("get_ducked"), -1));
			get_mounted_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_(""), _("ModelState")), _("get_mounted"), -1));
			get_crawling_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_(""), _("ModelState")), _("get_crawling"), -1));
			set_on_ladder_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_(""), _("ModelState")), _("set_onLadder"), -1));
			set_sprinting_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_(""), _("ModelState")), _("set_sprinting"), 1));
		}

		il2cpp_field_get(int, flags, flags_ptr);
		il2cpp_field_set(int, set_flags, flags_ptr);

		inline void remove_flag(int flag) {
			auto player_flags = flags();
			player_flags &= ~flag;
			set_flags(player_flags);
		}

		inline void add_flag(int flag) {
			auto player_flags = flags();
			set_flags(player_flags |= flag);
		}

		inline bool has_flag(int flag) {
			auto player_flags = flags();
			return (player_flags & flag);
		}

		inline void set_sprinting(bool state) {
			safe.call<void>(set_sprinting_ptr, this, state);
		}

		inline void set_on_ladder(bool state) {
			safe.call<void>(set_on_ladder_ptr, this, state);
		}

		inline bool ducked() {
			return safe.call<bool>(get_ducked_ptr, this);
		}

		inline bool crawling() {
			return safe.call<bool>(get_crawling_ptr, this);
		}

		inline bool mounted() {
			return safe.call<bool>(get_mounted_ptr, this);
		}
	};
}
