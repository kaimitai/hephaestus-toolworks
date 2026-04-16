#include <algorithm>
#include <format>
#include <iostream>
#include <core/klib/Kfile.h>
#include <core/script/ScriptLoader.h>
#include <string>

int main(int argc, char** argv) try {

	const auto rom{ klib::file::read_file_as_bytes("c:/temp/boo/boo-us.nes") };

	script::ScriptLoader loader;
	loader.parse_rom(rom, 5, 0x8000);
	auto layer{ loader.get_script_layer() };

	std::vector<std::size_t> addrs;
	for (const auto& [addr, _] : layer.instructions)
		addrs.push_back(addr);

	std::sort(addrs.begin(), addrs.end());

	std::unordered_set<std::size_t> labels;

	// entrypoints
	for (const auto& world : layer.ptr_table)
		for (auto ep : world)
			labels.insert(ep);

	// jump targets
	for (const auto& [_, instr] : layer.instructions)
		if (instr.jump_target)
			labels.insert(*instr.jump_target);

	// entrypoint reverse map
	std::unordered_map<std::size_t, std::vector<std::pair<std::size_t, std::size_t>>> ep_map;

	for (std::size_t w = 0; w < layer.ptr_table.size(); ++w) {
		for (std::size_t r = 0; r < layer.ptr_table[w].size(); ++r) {
			ep_map[layer.ptr_table[w][r]].emplace_back(w, r);
		}
	}

	std::string asmfile;

	for (auto addr : addrs) {

		// print entrypoint annotations
		if (ep_map.contains(addr)) {
			for (auto [w, r] : ep_map[addr]) {
				asmfile += std::format("; W{} R{}\n", w, r);
			}
		}

		if (labels.contains(addr))
			asmfile += std::format("@loc_{:04x}:\n", addr);

		const auto& instr = layer.instructions.at(addr);
		const auto& op = layer.opcodes.at(instr.opcode);

		asmfile += std::format("\t{}", op.mnemonic);

		if (instr.arg)
			asmfile += std::format(" {}", instr.arg.value());
		if (instr.jump_target)
			asmfile += std::format(" @loc_{:04x}", instr.jump_target.value());
		if (instr.string)
			asmfile += std::format(" \"{}\"", instr.string.value());

		asmfile += "\n";
	}

	klib::file::write_string_to_file(asmfile, "c:/temp/boo/boo-us.asm");
	return 0;
}
catch (const std::exception& ex) {
	std::cout << ex.what() << "\n";
	return 1;
}
