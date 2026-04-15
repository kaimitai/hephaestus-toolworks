#include <format>
#include <iostream>
#include <core/klib/Kfile.h>
#include <core/rom/Rom_manager.h>

int main(int argc, char** argv) {

	const auto rom{ klib::file::read_file_as_bytes("c:/temp/boo/boo-us.nes") };
	rom::ROM_Manager mgr(8);

	auto script_ptrs_addr{ mgr.read_word(rom, 5, 0x8000) };
	auto script_ptr_count{ mgr.get_ptr_table_size(rom, 5, script_ptrs_addr) };

	for (std::size_t i{ 0 }; i < script_ptr_count; ++i) {
		auto sub_ptrs{ mgr.read_word(rom,5,script_ptrs_addr + 2 * i) };
		auto sub_ptr_count{ mgr.get_ptr_table_size(rom, 5, sub_ptrs) };

		std::cout << i << ": " << sub_ptr_count << "\n";
	}

	return 0;
}
