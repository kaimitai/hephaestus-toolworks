#include <algorithm>
#include <format>
#include <iostream>
#include <core/klib/Kfile.h>
#include <core/script/ScriptLoader.h>
#include <core/script/ScriptAssembler.h>
#include <string>

int main(int argc, char** argv) try {

	const auto rom{ klib::file::read_file_as_bytes("c:/temp/boo/boo-us.nes") };

	script::ScriptLoader loader;
	loader.parse_rom(rom, 5, 0x8000);
	auto layer{ loader.get_normalized_script_layer() };

	// return 0;

	script::ScriptAssembler assembler;
	auto bytes{ assembler.assemble(layer, 0x98ca) };

	// 0x158da
	auto xom{ rom };
	for (std::size_t i{ 0 }; i < bytes.size(); ++i)
		xom.at(0x158da + i) = bytes[i];

	klib::file::write_bytes_to_file(xom, "c:/temp/boo/boo-us-out.nes");

	return 0;
}
catch (const std::exception& ex) {
	std::cout << ex.what() << "\n";
	return 1;
}
