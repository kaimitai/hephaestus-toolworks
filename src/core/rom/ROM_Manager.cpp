#include "ROM_Manager.h"
#include <algorithm>
#include <format>
#include <stdexcept>

rom::ROM_Manager::ROM_Manager(std::size_t p_bank_count) {
	if (p_bank_count == 0)
		throw std::runtime_error("No banks defined");

	for (std::size_t i{ 0 }; i < p_bank_count; ++i)
		cpu_bases.push_back(0x8000);

	cpu_bases.back() = 0xc000;
}

word rom::ROM_Manager::read_word(const std::vector<byte>& p_rom,
	std::size_t p_offset) const {
	return p_rom.at(p_offset + 1) * 256 + p_rom.at(p_offset);
}

word rom::ROM_Manager::read_word(const std::vector<byte>& p_rom, std::size_t p_bank,
	cpu_addr p_cpu_addr) const {
	return read_word(p_rom, cpu_addr_to_rom_offset(p_bank, p_cpu_addr));
}

std::size_t rom::ROM_Manager::cpu_addr_to_rom_offset(std::size_t p_bank_no,
	cpu_addr p_cpu_addr) const {
	if (p_bank_no >= cpu_bases.size())
		throw std::runtime_error(
			std::format("Invalid bank no: {} (must be 0-{})", p_bank_no, cpu_bases.size() - 1)
		);
	const auto cpu_base{ cpu_bases[p_bank_no] };

	if (p_cpu_addr < cpu_base || p_cpu_addr >= cpu_base + 0x4000)
		throw std::runtime_error(
			std::format("Invalid cpu address: ${:04x} (must be ${:04x}-${:04x})",
				p_cpu_addr, cpu_base, cpu_base + 0x4000 - 1)
		);

	std::size_t bank_start{ 0x10 + 0x4000 * p_bank_no };

	return bank_start + p_cpu_addr - cpu_base;
}

std::size_t rom::ROM_Manager::get_ptr_table_size(const std::vector<byte>& p_rom,
	std::size_t p_bank_no, cpu_addr p_cpu_addr) const {
	word min_target{ read_word(p_rom, p_bank_no, p_cpu_addr) };
	word cursor{ p_cpu_addr };
	std::size_t result{ 0 };

	while (cursor < min_target) {
		++result;
		min_target = std::min(min_target, read_word(p_rom, p_bank_no, cursor));
		cursor += 2;
	}

	return result;
}
