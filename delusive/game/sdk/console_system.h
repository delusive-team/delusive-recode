#pragma once
#include "../il2cpp/il2cpp.h"

namespace sdk {
	struct ConsoleSystemOption {
		bool is_server;
		bool is_client;
		bool forward_to_server_on_missing;
		bool print_output;
		bool is_unrestricted;
		bool from_rcon;
		bool is_from_server;

		static ConsoleSystemOption* client() {
			static ConsoleSystemOption opt = { false, true, true, true, false, false, false };
			return &opt;
		}

		static ConsoleSystemOption* server() {
			static ConsoleSystemOption opt = { true, false, false, true, true, false, true };
			return &opt;
		}
	};

	class ConsoleSystem {
	private:
		static inline uintptr_t run_ptr = 0;

	public:
		static void initialize() {
			auto klass = il2cpp::get_class(_(""), _("ConsoleSystem"));
			run_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(klass, _("Run"), 3));
		}

		static inline void run(ConsoleSystemOption* options, const char* command) {
			auto cmd_string = il2cpp::new_string(command);
			if (!cmd_string)
				return;

			safe.call<void>(run_ptr, options, cmd_string, 0);
		}

		static inline void run_client(const char* command) {
			run(ConsoleSystemOption::client(), command);
		}

		static inline void run_server(const char* command) {
			run(ConsoleSystemOption::server(), command);
		}
	};
}
