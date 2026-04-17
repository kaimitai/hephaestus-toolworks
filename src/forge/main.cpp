#include <algorithm>
#include <format>
#include <iostream>
#include <core/klib/Kfile.h>
#include <core/script/ScriptLoader.h>
#include <core/script/ScriptString.h>
#include <string>

int main(int argc, char** argv) try {

	const auto rom{ klib::file::read_file_as_bytes("c:/temp/boo/boo-us.nes") };

	script::ScriptLoader loader;
	loader.parse_rom(rom, 5, 0x8000);
	auto layer{ loader.get_normalized_script_layer() };

	return 0;
}
catch (const std::exception& ex) {
	std::cout << ex.what() << "\n";
	return 1;
}
