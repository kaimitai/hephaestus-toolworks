#include "ScriptAssembler.h"
#include "ScriptString.h"
#include <algorithm>
#include <cassert>
#include <format>
#include <map>
#include <optional>
#include <stdexcept>
#include <unordered_map>

std::vector<byte> script::ScriptAssembler::assemble(const ScriptLayer& layer, cpu_addr addr) const {
	const auto dense_ptr_table{ build_dense_ptr_table(layer) };

	// reserve space for ptr table and store some sizes for ptr table emission later
	std::size_t world_count{ dense_ptr_table.size() };
	std::vector<std::size_t> room_counts;

	std::size_t ptr_count{ world_count };
	for (const auto& w : dense_ptr_table) {
		ptr_count += w.size();
		room_counts.push_back(w.size());
	}

	std::vector<byte> result(2 * ptr_count, 0x00);
	std::size_t offset{ addr + 2 * ptr_count };

	std::vector<std::size_t> string_to_offset, instr_to_offset;
	// calc instruction offsets
	for (const auto& instr : layer.instructions) {
		instr_to_offset.push_back(offset);
		offset += instr.size();
	}

	// lay down strings and store the offset to each index
	for (std::size_t i{ 0 }; i < layer.strings.size(); ++i) {
		string_to_offset.push_back(offset);
		const auto scriptstr{ ScriptString(layer.strings[i]) };
		const auto bytes{ scriptstr.to_bytes() };
		offset += bytes.size();
	}

	// we have all instruction offsets and can emit bytes
	for (std::size_t i{ 0 }; i < layer.instructions.size(); ++i) {
		const auto& instr{ layer.instructions[i] };
		std::optional<byte> jmp_target;
		std::optional<cpu_addr> string_ref;

		if (instr.jump_target) {
			auto target = instr_to_offset[*instr.jump_target];
			auto current = instr_to_offset[i];

			if (target <= current)
				throw std::runtime_error("Backward or zero jump");

			std::size_t delta = target - current - instr.size() + 1;

			if (delta == 0 || delta > 255)
				throw std::runtime_error("Invalid jump");

			jmp_target = static_cast<byte>(delta);
		}
		if (instr.string_ref)
			string_ref = static_cast<cpu_addr>(string_to_offset.at(*instr.string_ref));

		const auto bytes{ instr.to_bytes(jmp_target, string_ref) };

		assert(bytes.size() == instr.size());

		result.insert(end(result), begin(bytes), end(bytes));
	}

	// emit outlined strings last
	for (std::size_t i{ 0 }; i < layer.strings.size(); ++i) {
		const auto scriptstr{ ScriptString(layer.strings[i]) };
		const auto bytes{ scriptstr.to_bytes() };
		result.insert(end(result), begin(bytes), end(bytes));
	}

	// patch world ptr table
	uint32_t cur = addr + 2 * world_count;
	for (std::size_t w{ 0 }; w < world_count; ++w) {
		cpu_addr l_addr = static_cast<cpu_addr>(cur);
		result[2 * w] = l_addr & 0xFF;
		result[2 * w + 1] = l_addr >> 8;
		cur += 2 * room_counts[w];
	}

	// patch room ptr tables
	std::size_t idx{ 2 * world_count }; // start right after world ptr table

	for (std::size_t w{ 0 }; w < world_count; ++w) {
		for (std::size_t r{ 0 }; r < room_counts[w]; ++r) {

			std::size_t instr_idx = dense_ptr_table[w][r];
			cpu_addr l_addr = static_cast<cpu_addr>(instr_to_offset[instr_idx]);

			result[idx++] = l_addr & 0xFF;
			result[idx++] = l_addr >> 8;
		}
	}

	return result;
}

std::vector<std::vector<std::size_t>> script::ScriptAssembler::build_dense_ptr_table(
	const ScriptLayer& layer) const {
	// find max world
	std::size_t max_world{ 0 };
	for (const auto& [instr_idx, refs] : layer.ptr_table) {
		for (const auto& [w, r] : refs)
			max_world = std::max(max_world, w);
	}

	std::vector<std::vector<std::size_t>> table(max_world + 1);

	// size each world's room vector
	for (const auto& [instr_idx, refs] : layer.ptr_table) {
		for (const auto& [w, r] : refs) {
			if (table[w].size() <= r)
				table[w].resize(r + 1, SIZE_MAX); // sentinel
		}
	}

	// fill
	for (const auto& [instr_idx, refs] : layer.ptr_table) {
		for (const auto& [w, r] : refs) {
			if (table[w][r] != SIZE_MAX)
				throw std::runtime_error(
					std::format("Duplicate ptr table entry: w={},r={}", w, r)
				);

			table[w][r] = instr_idx;
		}
	}

	// validate density (no holes)
	for (std::size_t w = 0; w < table.size(); ++w) {
		for (std::size_t r = 0; r < table[w].size(); ++r) {
			if (table[w][r] == SIZE_MAX) {
				throw std::runtime_error(
					"Sparse ptr table at world " + std::to_string(w) +
					", room " + std::to_string(r)
				);
			}
		}
	}

	return table;
}
