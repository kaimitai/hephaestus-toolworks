#ifndef SCRIPT_SCRIPTLOADER_H
#define SCRIPT_SCRIPTLOADER_H

#include "ScriptString.h"
#include "Opcode.h"
#include "Instruction.h"
#include "./../rom/ROM_Manager.h"
#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using byte = uint8_t;

namespace script {

	struct NormalizedScriptLayer {
		// maps instruction index to vector of (world, room) having that instruction as entrypt
		std::unordered_map<std::size_t, std::vector<std::pair<std::size_t, std::size_t>>> ptr_table;
		// sequential instructions; instructions with string refs have index values that reference...
		std::vector<Instruction> instructions;
		// ...the actual strings
		std::vector<std::string> strings;
	};

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
		NormalizedScriptLayer get_normalized_script_layer(void) const;
	};

}

#endif
