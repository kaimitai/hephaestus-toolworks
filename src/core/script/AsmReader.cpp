#include "AsmReader.h"
#include "script_constants.h"
#include "./../klib/Kstring.h"
#include "./../klib/Kfile.h"
#include <cstdint>
#include <format>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

using byte = uint8_t;

script::ScriptLayer script::AsmReader::assemble(const ht::ScriptConfig& config,
	const std::string& asm_filename) const {

	std::vector<std::string> lines;
	std::unordered_map<std::string, byte> defines, mnemonics;
	std::unordered_map<std::size_t, std::vector<std::pair<std::size_t, std::size_t>>> instr_to_ptr;
	std::unordered_map<std::string, std::size_t> label_to_instr;
	std::unordered_map<std::size_t, std::string> instr_jump_to_label;
	std::unordered_map<std::string, std::size_t> ref_strings;

	{
		bool is_defines{ true };
		auto all_lines{ klib::file::read_file_as_strings(asm_filename) };

		for (const auto& line : all_lines) {
			auto str{ klib::str::trim(klib::str::strip_comment(line)) };
			if (str.empty())
				continue;

			if (str == c::ASM_SECTION_DEFINES) {
				is_defines = true;
				continue;
			}
			else if (str == c::ASM_SECTION_SCRIPT) {
				is_defines = false;
				continue;
			}

			if (is_defines) {
				const auto define{ klib::str::parse_define(str) };
				defines.emplace(define.first, static_cast<byte>(klib::str::parse_numeric(define.second)));
			}
			else {
				lines.push_back(str);
			}
		}
	}

	// create mnemonic lookup map
	for (const auto& opcode : config.opcodes)
		mnemonics[klib::str::to_lower(opcode.second.mnemonic)] = opcode.first;

	ScriptLayer result;
	std::vector<Instruction> instrs;

	for (const auto& line : lines) {
		if (klib::str::str_begins_with(line, std::string(c::ASM_DIRECTIVE_ENTRYPOINT))) {
			const auto kv{ klib::str::split_whitespace(line) };
			if (kv.size() != 2)
				throw std::runtime_error(std::format("Invalid entrypoint line: {}", line));
			const auto rw{ klib::str::split_string(kv[1], '.') };
			if (rw.size() != 2)
				throw std::runtime_error(std::format("Invalid entrypoint indexes: {}", line));
			instr_to_ptr[instrs.size()].push_back(std::make_pair(
				static_cast<std::size_t>(klib::str::parse_numeric(rw[0])),
				static_cast<std::size_t>(klib::str::parse_numeric(rw[1]))
			));
		}
		else if (klib::str::str_begins_with(line, "@")) {
			if (line.back() != ':')
				throw std::runtime_error(std::format("Invalid label definition: {}", line));
			if (klib::str::split_whitespace(line).size() != 1)
				throw std::runtime_error(std::format("Invalid label definition: {}", line));
			std::string label{ line };
			label.pop_back();
			if (label_to_instr.contains(label))
				throw std::runtime_error(std::format("Label {} redefined on line: {}", label, line));
			label_to_instr[label] = instrs.size();
		}
		else {
			// we have an instruction
			const auto tokens{ klib::str::split_whitespace(line) };
			std::string mnemo{ klib::str::to_lower(tokens.at(0)) };
			if (!mnemonics.contains(mnemo))
				throw std::runtime_error(
					std::format("Invalid opcode '{}' on line: {}", tokens[0], line)
				);

			Instruction instr{
				.opcode = mnemonics.at(mnemo)
			};
			const auto& opcode{ config.opcodes.at(instr.opcode) };

			std::optional<byte> operand;
			std::optional<std::string> string;
			std::optional<std::size_t> string_ref;

			if (opcode.argType != ArgType::None) {

				if (tokens.size() < 2)
					throw std::runtime_error(std::format("Missing operand on line: {}", line));

				if (opcode.argType == ArgType::Byte) {
					if (defines.contains(tokens[1]))
						operand = defines.at(tokens[1]);
					else
						operand = static_cast<byte>(klib::str::parse_numeric(tokens[1]));
				}
				else if (opcode.argType == ArgType::ImmediateString ||
					opcode.argType == ArgType::PointerString) {
					if (tokens[1].size() < 2 || tokens[1].back() != '"' || tokens[1][0] != '"')
						throw std::runtime_error(std::format("invalid string operand '{}' operand on line: {}", tokens[1], line));

					std::string scriptstr{ tokens[1].substr(1, tokens[1].size() - 2) };

					if (opcode.argType == ArgType::ImmediateString)
						string = scriptstr;
					else {
						if (ref_strings.contains(scriptstr))
							string_ref = ref_strings.at(scriptstr);
						else {
							auto refidx{ ref_strings.size() };
							ref_strings[scriptstr] = refidx;
							string_ref = refidx;
						}
					}
				}
			}

			if (opcode.flow == Flow::ConditionalJump || opcode.flow == Flow::Jump) {
				if (tokens.size() < 2 || tokens.back().at(0) != '@')
					throw std::runtime_error(std::format("Invalid jump target on line: {}", line));

				std::string label{ tokens.back() };
				instr_jump_to_label[instrs.size()] = label;
			}

			instr.arg = operand;
			instr.string = string;
			instr.string_ref = string_ref;

			instrs.push_back(instr);
		}
	}

	// second pass; label resolution
	for (const auto& kv : instr_jump_to_label) {
		if (!label_to_instr.contains(kv.second))
			throw std::runtime_error(std::format("Referenced label {} not defined", kv.second));
		instrs.at(kv.first).jump_target = label_to_instr.at(kv.second);
	}

	result.instructions = instrs;
	result.ptr_table = instr_to_ptr;
	result.strings = std::vector<std::string>(ref_strings.size());

	for (const auto& [str, idx] : ref_strings) {
		result.strings.at(idx) = str;
	}

	return result;
}
