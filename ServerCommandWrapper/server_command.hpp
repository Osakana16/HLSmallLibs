#include <vector>
#include <unordered_map>
#include <string>
#include <functional>

#include <extdll.h>
#include <dllapi.h>
#include <meta_api.h>
#include <entity_state.h>

namespace hlutil {
	using CommandFunction = std::function<void(const std::vector<std::string>&)>;
	class CommandManager final {
		CommandManager() = delete;
		CommandManager(const CommandManager&) = delete;
		CommandManager(CommandManager&&) = delete;
		CommandManager& operator=(const CommandManager&) = delete;

		static std::unordered_map<std::string, CommandFunction> commands;
	public:
		static inline void insert_command(const std::string& command_name, CommandFunction command) noexcept {
			commands.insert({ command_name, command });
		}

		static inline void register_command() noexcept {
			 static auto GetArgs = []() noexcept {
				static std::vector<std::string> args{};
				for (int i = 1; ; i++) {
					auto arg = CMD_ARGV(i);
					if (arg == nullptr || strlen(arg) <= 0) {
						break;
					}
					args.push_back(arg);
				}
				return std::move(args);
			};
			static std::string command_name{};
			for (auto& command : commands) {
				command_name = command.first;
				REG_SVR_COMMAND(
					command_name.c_str(),
					[] () noexcept { commands.at(command_name)(GetArgs()); }
				);
			}
		}
	};
}