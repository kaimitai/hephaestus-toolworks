#ifndef SCRIPT_SCRIPTLOADER_H
#define SCRIPT_SCRIPTLOADER_H

#include "ScriptString.h"
#include "Opcode.h"
#include "Instruction.h"
#include "./../rom/ROM_Manager.h"
#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using byte = uint8_t;

namespace script {

	struct ScriptLayer {
		std::vector<std::vector<std::size_t>> ptr_table;
		std::unordered_map<std::size_t, Instruction> instructions;
		std::unordered_map<byte, Opcode> opcodes;
	};

	class ScriptLoader {

		std::unordered_set<std::size_t> visited;
		std::unordered_map<byte, Opcode> opcodes;
		std::vector<std::vector<std::size_t>> ptr_table;
		std::unordered_map<std::size_t, Instruction> instructions;
		rom::ROM_Manager rom_mgr;
		CharacterTables tables;

		void parse_blob_from_entrypoint(const std::vector<byte>& p_rom,
			std::size_t p_bank_no, std::size_t p_rom_offset);

	public:
		ScriptLoader(void);
		void initialize_opcodes(void);
		void parse_rom(const std::vector<byte>& p_rom,
			std::size_t p_master_ptr_bank, std::size_t p_master_ptr_cpu_offset);
		ScriptLayer get_script_layer(void) const;
	};

}

#endif
