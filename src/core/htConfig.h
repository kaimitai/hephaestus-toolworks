#ifndef HTCONFIG_H
#define HTCONFIG_H

#include <map>
#include <string>
#include <unordered_map>
#include "./script/Opcode.h"

namespace ht {

	struct ScriptConfig {
		std::unordered_map<byte, script::Opcode> opcodes;
		std::map<script::ArgDomain, std::map<byte, std::string>> defines;
	};

	class htConfig {

		ScriptConfig scriptConfig;

	public:
		htConfig(void);
		const ScriptConfig& get_script_config(void) const;
	};

}

#endif
