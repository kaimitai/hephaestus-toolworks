#include "ScriptLoader.h"
#include "./../rom/BitReader.h"
#include <algorithm>
#include <cstdint>
#include <format>
#include <stdexcept>

using word = uint16_t;
using cpu_addr = uint16_t;

script::ScriptLoader::ScriptLoader(void) :
	rom_mgr{ rom::ROM_Manager(8) }
{
	initialize_opcodes();
}

void script::ScriptLoader::parse_rom(const std::vector<byte>& p_rom,
	std::size_t p_master_ptr_bank, std::size_t p_master_ptr_cpu_addr) {

	auto world_ptr_table{ rom_mgr.read_word(p_rom, p_master_ptr_bank, p_master_ptr_cpu_addr) };
	auto world_ptr_count{ rom_mgr.get_ptr_table_size(p_rom, p_master_ptr_bank, world_ptr_table) };

	// get all world room ptrs
	std::vector<cpu_addr> world_room_ptrs;
	for (std::size_t i{ 0 }; i < world_ptr_count; ++i)
		world_room_ptrs.push_back(
			rom_mgr.read_word(p_rom, p_master_ptr_bank, world_ptr_table + 2 * i)
		);

	// the first world's room ptr table spans the entire contiguous room-ptr block
	auto total_room_ptr_count{
		rom_mgr.get_ptr_table_size(p_rom, p_master_ptr_bank, world_room_ptrs.at(0))
	};
	cpu_addr room_ptr_block_end{
		static_cast<cpu_addr>(world_room_ptrs[0] + total_room_ptr_count * 2)
	};

	for (std::size_t i{ 0 }; i < world_ptr_count; ++i) {
		cpu_addr room_ptrs{ world_room_ptrs[i] };
		cpu_addr next_room_ptrs{
			(i + 1 < world_ptr_count) ? world_room_ptrs[i + 1] : room_ptr_block_end
		};

		std::size_t room_ptr_count{ static_cast<std::size_t>((next_room_ptrs - room_ptrs) / 2) };

		for (std::size_t j{ 0 }; j < room_ptr_count; ++j) {
			auto room_script_cpu_offset{
				rom_mgr.read_word(p_rom, p_master_ptr_bank, room_ptrs + 2 * j)
			};

			std::size_t rom_ptr_offset{
				rom_mgr.cpu_addr_to_rom_offset(p_master_ptr_bank, room_script_cpu_offset)
			};

			ptr_table[rom_ptr_offset].push_back(std::make_pair(i, j));
			parse_blob_from_entrypoint(p_rom, p_master_ptr_bank, rom_ptr_offset);
		}
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

			instr.string_ref = rom_ptr;

			if (!strings.contains(rom_ptr)) {
				ScriptString str(p_rom, rom_ptr);
				strings.insert(std::make_pair(rom_ptr, str.get_string()));
			}

			addr += 2;
		}
		else if (op.argType == ArgType::ImmediateString) {
			uint8_t token_count = p_rom.at(addr);

			ScriptString s(p_rom, addr);
			instr.string = s.get_string();

			// compute encoded size directly
			std::size_t byte_count = (token_count * 5 + 7) / 8;
			addr += 1 + byte_count;
		}

		instructions[instr_addr] = std::move(instr);

		// sript ends here
		if (op.flow == Flow::End)
			return;
	}
}

script::NormalizedScriptLayer script::ScriptLoader::get_normalized_script_layer(void) const {

	// sort all instruction and string offsets
	std::vector<std::size_t> offsets, string_offsets;
	for (const auto& [offset, _] : instructions)
		offsets.push_back(offset);
	for (const auto& [offset, _] : strings)
		string_offsets.push_back(offset);

	std::sort(begin(offsets), end(offsets));
	std::sort(begin(string_offsets), end(string_offsets));

	std::unordered_map<uint32_t, size_t> offset_to_index, string_offset_to_idx;

	std::vector<Instruction> norm_instructions;
	std::unordered_map<std::size_t, std::vector<std::pair<std::size_t, std::size_t>>> norm_ptr_table;
	std::vector<std::string> norm_strings;

	// make a vector of instructions
	for (size_t i = 0; i < offsets.size(); ++i) {
		uint32_t off = offsets[i];
		offset_to_index[off] = i;
		norm_instructions.push_back(instructions.at(off));
	}

	// make a vector of strings
	for (size_t i = 0; i < string_offsets.size(); ++i) {
		uint32_t off = string_offsets[i];
		string_offset_to_idx[off] = i;
		norm_strings.push_back(strings.at(off));
	}

	// normalize jump targets and string refs
	for (auto& instr : norm_instructions) {
		if (instr.jump_target)
			instr.jump_target = offset_to_index[instr.jump_target.value()];
		if (instr.string_ref)
			instr.string_ref = string_offset_to_idx[instr.string_ref.value()];
	}

	// normalize ptr table
	for (const auto& kv : ptr_table)
		norm_ptr_table[offset_to_index[kv.first]] = kv.second;

	return script::NormalizedScriptLayer{
		.ptr_table = norm_ptr_table,
		.instructions = norm_instructions,
		.strings = norm_strings
	};
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
