#ifndef CORE_ROM_MANAGER_H
#define CORE_ROM_MANAGER_H

#include <cstdint>
#include <vector>

using byte = uint8_t;
using word = uint16_t;
using cpu_addr = uint16_t;

namespace rom {

	class ROM_Manager {

		std::vector<word> cpu_bases;

	public:
		ROM_Manager(std::size_t p_bank_count);
		std::size_t cpu_addr_to_rom_offset(std::size_t p_bank_no, cpu_addr p_cpu_addr) const;

		word read_word(const std::vector<byte>& p_rom, std::size_t p_offset) const;
		word read_word(const std::vector<byte>& p_rom, std::size_t p_bank, cpu_addr p_cpu_addr) const;

		std::size_t get_ptr_table_size(const std::vector<byte>& p_rom,
			std::size_t p_bank_no, cpu_addr p_cpu_addr) const;
	};

}

#endif
