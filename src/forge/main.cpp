#include <format>
#include <iostream>
#include <core/klib/Kfile.h>
#include <core/rom/Rom_manager.h>
#include <core/script/ScriptString.h>

int main(int argc, char** argv) try {

	const auto rom{ klib::file::read_file_as_bytes("c:/temp/boo/boo-us.nes") };
	rom::ROM_Manager mgr(8);

	script::CharacterTables tables;
	tables.initialize_default();
	script::ScriptString str(rom,
		mgr.cpu_addr_to_rom_offset(5, 0x9e53),
		tables);

	std::cout << "\"" << str.get_string() << "\"\n" ;

	return 0;


	auto script_ptrs_addr{ mgr.read_word(rom, 5, 0x8000) };
	auto script_ptr_count{ mgr.get_ptr_table_size(rom, 5, script_ptrs_addr) };

	std::cout << std::format("Script ptrs: {:04x}", script_ptrs_addr) << "\n";

	for (std::size_t i{ 0 }; i < script_ptr_count; ++i) {
		auto sub_ptrs{ mgr.read_word(rom,5,script_ptrs_addr + 2 * i) };
		auto sub_ptr_count{ mgr.get_ptr_table_size(rom, 5, sub_ptrs) };

		auto offset{ mgr.read_word(rom, 5, sub_ptrs) };

		std::cout << i << ": " << sub_ptr_count << " - First: " <<
			std::format("{:04x}", offset)
			<< "\n";
	}

	std::size_t stringstart{ mgr.cpu_addr_to_rom_offset(5, 0x9e38) };
	std::cout << std::format("\n\nString start: {:04x}", stringstart);

	return 0;
}
catch (const std::exception& ex) {
	std::cout << ex.what() << "\n";
	return 1;
}
