#ifndef SCRIPT_SCRIPTLOADER_H
#define SCRIPT_SCRIPTLOADER_H

#include "ScriptString.h"
#include "Opcode.h"
#include "Instruction.h"
#include "ScriptLayer.h"
#include "./../rom/ROM_Manager.h"
#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using byte = uint8_t;

namespace script {

	class ScriptLoader {

		std::unordered_set<std::size_t> visited;
		std::unordered_map<byte, Opcode> opcodes;
		rom::ROM_Manager rom_mgr;

		std::unordered_map<std::size_t, std::vector<std::pair<std::size_t, std::size_t>>> ptr_table;
		std::unordered_map<std::size_t, Instruction> instructions;
		std::unordered_map<std::size_t, std::string> strings;

		void parse_blob_from_entrypoint(const std::vector<byte>& p_rom,
			std::size_t p_bank_no, std::size_t p_rom_offset);

	public:
		ScriptLoader(void);
		void initialize_opcodes(void);
		void parse_rom(const std::vector<byte>& p_rom,
			std::size_t p_master_ptr_bank, std::size_t p_master_ptr_cpu_offset);
		ScriptLayer get_normalized_script_layer(void) const;
	};

}

#endif
