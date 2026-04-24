#include "Cli.h"

#include <algorithm>
#include <format>
#include <iostream>
#include <core/klib/Kfile.h>
#include <core/klib/KString.h>
#include <core/htConfig.h>
#include <core/script/ScriptLoader.h>
#include <core/script/ScriptAssembler.h>
#include <core/script/AsmView.h>
#include <core/script/AsmWriter.h>
#include <core/script/AsmReader.h>
#include <core/app_constants.h>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

constexpr std::size_t SCRIPT_BANK_NO{ 5 }, BANK_COUNT{ 8 };
constexpr cpu_addr SCRIPT_MASTER_PTR{ 0x8000 };

forge::Cli::Cli(int argc, char** argv) {

	std::cout << ht::appc::APP_NAME << " version " << ht::appc::APP_VERSION << " (" <<
		ht::appc::APP_NAME_FORGE << ")\n" << ht::appc::APP_URL << "\n\nBy Kai E. Fr";
	output_oe_on_windows();
	std::cout << "land <kai.froland@gmail.com>\nBuild date : " <<
		__DATE__ << " " << __TIME__ << " CET\n\n";

	if (argc != 4) {
		std::cout << "Usage:\n  forge x <input.nes> <output.asm>    Extract scripting layer from ROM\n" <<
			"  forge b <input.asm> <output.nes>    Build and patch ROM from ASM\n\nSee the documentation for details\n\n";
		return;
	}

	std::string cmd{ argv[1] }, infile{ argv[2] }, outfile{ argv[3] };
	ht::htConfig config;

	if (cmd == "x") {
		std::cout << "Reading nes file " << infile << "...\n";
		const auto rom{ klib::file::read_file_as_bytes(infile) };
		script::ScriptLoader loader;
		loader.parse_rom(rom, config.get_script_config().opcodes, SCRIPT_BANK_NO, SCRIPT_MASTER_PTR);
		script::AsmView view(config.get_script_config(), loader.get_normalized_script_layer());
		script::AsmWriter writer;
		writer.write_asm_file(config.get_script_config(), view, outfile);
		std::cout << "Script layer of " << infile << " written to asm file " << outfile << "\n\n";
	}
	else if (cmd == "b") {
		std::cout << "Reading nes file " << outfile << "...\n";
		auto rom{ klib::file::read_file_as_bytes(outfile) };
		std::cout << "Reading - and trying to assemble - asm file " << infile << "...\n";
		script::AsmReader reader;
		auto layer{ reader.assemble(config.get_script_config(), infile) };

		rom::ROM_Manager mgr(BANK_COUNT);
		cpu_addr addr{ mgr.read_word(rom, SCRIPT_BANK_NO, SCRIPT_MASTER_PTR) };

		script::ScriptAssembler assembler;
		auto bytes{ assembler.assemble(layer, addr) };

		std::size_t used_space{ bytes.size() };
		std::size_t avail_space{ 0xc000 - static_cast<std::size_t>(addr) };
		float pct{
			100.0f * (static_cast<float>(used_space) / static_cast<float>(avail_space))
		};

		std::cout << "Used space: " << used_space << " of " << avail_space <<
			std::format(" available bytes ({:0.2f}%)\n", pct);

		if (used_space > avail_space) {
			std::cout << "Script layer too big to fit in bank\n\n";
			return;
		}

		std::cout << std::format("Injecting ptr table hierarchy and script/strings blob starting from addr {:02x}:{:04x}\n",
			SCRIPT_BANK_NO, addr);

		auto rom_offset{ mgr.cpu_addr_to_rom_offset(SCRIPT_BANK_NO, addr) };
		for (std::size_t i{ 0 }; i < bytes.size(); ++i)
			rom.at(i + rom_offset) = bytes[i];

		std::string patchedfile{ get_final_out_nes_file(outfile) };
		klib::file::write_bytes_to_file(rom, patchedfile);
		std::cout << "Patched ROM written to " << patchedfile << "!\n\n";
	}
	else
		throw std::runtime_error(std::format("Invalid command: {}", cmd));
}

std::string forge::Cli::get_final_out_nes_file(const std::string& p_out_file) const {
	std::string result{ p_out_file };

	if (p_out_file.size() >= 4) {
		std::string ext{ p_out_file.substr(p_out_file.size() - 4) };
		if (klib::str::str_equals_icase(ext, ".nes"))
			result = p_out_file.substr(0, p_out_file.size() - 4);
	}

	result += "-out.nes";
	return result;
}

// sad that this is needed in 2026
void forge::Cli::output_oe_on_windows(void) const {
#ifdef _WIN32
	UINT old_cp = GetConsoleOutputCP();
	SetConsoleOutputCP(CP_UTF8);
#endif

	std::cout << "ø";

#ifdef _WIN32
	SetConsoleOutputCP(old_cp);
#endif
}
