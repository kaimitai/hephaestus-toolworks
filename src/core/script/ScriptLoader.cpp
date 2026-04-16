#include "ScriptLoader.h"
#include "./../rom/BitReader.h"
#include "CharacterTables.h"
#include <cstdint>
#include <format>
#include <stdexcept>

using word = uint16_t;
using cpu_addr = uint16_t;

script::ScriptLoader::ScriptLoader(void) :
	rom_mgr{ rom::ROM_Manager(8) }
{
	initialize_opcodes();
	tables.initialize_default();
}

script::ScriptLayer script::ScriptLoader::get_script_layer(void) const {
	return script::ScriptLayer{
		.ptr_table = ptr_table,
		.instructions = instructions,
		.opcodes = opcodes
	};
}

void script::ScriptLoader::parse_rom(const std::vector<byte>& p_rom,
	std::size_t p_master_ptr_bank, std::size_t p_master_ptr_cpu_addr) {

	auto world_ptr_table{ rom_mgr.read_word(p_rom, p_master_ptr_bank, p_master_ptr_cpu_addr) };
	auto world_ptr_count{ rom_mgr.get_ptr_table_size(p_rom, p_master_ptr_bank, world_ptr_table) };

	// for each world, make one vector
	for (std::size_t i{ 0 }; i < world_ptr_count; ++i) {
		std::vector<std::size_t> world_entrypoints;

		auto room_ptrs{ rom_mgr.read_word(p_rom,p_master_ptr_bank,world_ptr_table + 2 * i) };
		auto room_ptr_count{ rom_mgr.get_ptr_table_size(p_rom, p_master_ptr_bank, room_ptrs) };

		for (std::size_t j{ 0 }; j < room_ptr_count; ++j) {

			// make sure we don't treat previously parsed script code as a ptr
			std::size_t ptr_addr_rom_offset{ rom_mgr.cpu_addr_to_rom_offset(p_master_ptr_bank,
				room_ptrs + 2 * j) };
			if (instructions.contains(ptr_addr_rom_offset))
				break;

			auto room_script_cpu_offset{ rom_mgr.read_word(p_rom,p_master_ptr_bank,room_ptrs + 2 * j) };

			std::size_t rom_ptr_offset{
				rom_mgr.cpu_addr_to_rom_offset(p_master_ptr_bank,
					room_script_cpu_offset)
			};

			world_entrypoints.push_back(rom_ptr_offset);
			parse_blob_from_entrypoint(p_rom,
				p_master_ptr_bank, rom_ptr_offset);
		}

		ptr_table.push_back(world_entrypoints);
	}

}

void script::ScriptLoader::parse_blob_from_entrypoint(const std::vector<byte>& p_rom,
	std::size_t p_bank_no, std::size_t addr) {

	while (true) {

		if (visited.contains(addr))
			return;
		visited.insert(addr);

		std::size_t instr_addr = addr;

		Instruction instr{};

		byte opcode = p_rom.at(addr++);
		instr.opcode = opcode;

		auto it = opcodes.find(opcode);
		if (it == opcodes.end())
			throw std::runtime_error(
				std::format("Invalid opcode ${:02x} at ROM offset 0x{:05x}", opcode, addr - 1)
			);

		const auto& op = it->second;

		if (op.argType == ArgType::Byte)
			instr.arg = p_rom.at(addr++);

		if (op.flow == Flow::ConditionalJump || op.flow == Flow::Jump) {
			byte offset{ p_rom.at(addr++) };
			std::size_t target{ addr + offset - 1 };

			instr.jump_target = target;

			parse_blob_from_entrypoint(p_rom, p_bank_no, target);

			if (op.flow == Flow::Jump) {
				instructions[instr_addr] = std::move(instr);
				return;
			}
		}

		if (op.argType == ArgType::PointerString) {
			word cpu_ptr{ rom_mgr.read_word(p_rom, addr) };
			std::size_t rom_ptr{ rom_mgr.cpu_addr_to_rom_offset(5, cpu_ptr) };

			ScriptString str(p_rom, rom_ptr, tables);
			instr.string = str.get_string();

			addr += 2;
		}
		else if (op.argType == ArgType::ImmediateString) {
			uint8_t token_count = p_rom.at(addr);

			ScriptString s(p_rom, addr, tables);
			instr.string = s.get_string();

			// compute encoded size directly
			std::size_t byte_count = (token_count * 5 + 7) / 8;
			addr += 1 + byte_count;
		}

		instructions[instr_addr] = std::move(instr);

		// --- end ---
		if (op.flow == Flow::End)
			return;
	}
}

void script::ScriptLoader::initialize_opcodes(void) {
	opcodes = {
		{0x00, {"SetNPC", Flow::Continue, ArgType::Byte, ArgDomain::NPC}},
		{0x01, {"IfNotFlag", Flow::ConditionalJump, ArgType::Byte, ArgDomain::Flag}},
		{0x02, {"Jump", Flow::Jump, ArgType::None, ArgDomain::None}},
		{0x03, {"Msg", Flow::Continue, ArgType::PointerString, ArgDomain::None}},
		{0x04, {"End", Flow::End, ArgType::None, ArgDomain::None}},
		{0x05, {"SetFlag", Flow::Continue, ArgType::Byte, ArgDomain::Flag}},
		{0x06, {"ClearFlag", Flow::Continue, ArgType::Byte, ArgDomain::Flag}},
		{0x07, {"IfFlag", Flow::ConditionalJump, ArgType::Byte, ArgDomain::Flag}},
		{0x08, {"GetItem", Flow::Continue, ArgType::Byte, ArgDomain::Flag}},
		{0x09, {"MsgEnd", Flow::End, ArgType::ImmediateString, ArgDomain::None}},
		{0x0a, {"IfDecline", Flow::ConditionalJump, ArgType::None, ArgDomain::None}},
		{0x0b, {"IfSkins", Flow::ConditionalJump, ArgType::None, ArgDomain::None}},
		{0x0c, {"IfOlives", Flow::ConditionalJump, ArgType::Byte, ArgDomain::None}},
		{0x0d, {"PasswordEnd", Flow::End, ArgType::None, ArgDomain::None}},
		{0x0e, {"HealthEnd", Flow::End, ArgType::None, ArgDomain::None}},
		{0x0f, {"NectarEnd", Flow::End, ArgType::None, ArgDomain::None}}
	};
}
