#include <algorithm>
#include <format>
#include <iostream>
#include <core/klib/Kfile.h>
#include <core/htConfig.h>
#include <core/script/ScriptLoader.h>
#include <core/script/ScriptAssembler.h>
#include <core/script/AsmView.h>
#include <core/script/AsmWriter.h>
#include <string>

int main(int argc, char** argv) try {
	ht::htConfig config;

	const auto rom{ klib::file::read_file_as_bytes("c:/temp/boo/boo-eu.nes") };
	script::ScriptLoader loader;
	loader.parse_rom(rom, config.get_script_config().opcodes, 5, 0x8000);
	auto layer{ loader.get_normalized_script_layer() };
	script::AsmView view(config.get_script_config(), layer);
	script::AsmWriter writer;
	writer.write_asm_file(config.get_script_config(), view, "c:/temp/boo/boo-eu-new.asm");

	return 0;
	/*
	script::ScriptAssembler assembler;
	auto bytes{ assembler.assemble(layer, 0x98ca) };

	// 0x158da
	auto xom{ rom };
	for (std::size_t i{ 0 }; i < bytes.size(); ++i)
		xom.at(0x158da + i) = bytes[i];

	klib::file::write_bytes_to_file(xom, "c:/temp/boo/boo-us-out.nes");

	return 0;
	*/
}
catch (const std::exception& ex) {
	std::cout << ex.what() << "\n";
	return 1;
}
