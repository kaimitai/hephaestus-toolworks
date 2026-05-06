#ifndef HTCONFIG_H
#define HTCONFIG_H

#include <cstdint>
#include <map>
#include <string>
#include <unordered_map>
#include "./script/Opcode.h"

using byte = uint8_t;
using cpu_addr = uint16_t;

namespace ht {

	struct Address {
		byte bank;
		cpu_addr addr;
	};

	struct ScriptConfig {
		std::unordered_map<byte, script::Opcode> opcodes;
		std::map<script::ArgDomain, std::map<byte, std::string>> defines;
	};

	struct RGBColor {
		byte r, g, b;
	};

	class htConfig {

		ScriptConfig scriptConfig;
		std::unordered_map<std::string, Address> addresses;
		std::unordered_map<std::string, std::size_t> counts;
		std::vector<RGBColor> nes_palette;

	public:
		htConfig(void);
		const ScriptConfig& get_script_config(void) const;

		std::size_t count(const std::string& p_id) const;
		Address address(const std::string& p_id) const;
		const std::vector<RGBColor>& get_nes_palette(void) const;
	};

}

#endif
