#include "Instruction.h"
#include "ScriptString.h"

std::vector<byte> script::Instruction::to_bytes(std::optional<byte> p_jmp_target,
	std::optional<cpu_addr> p_string_ref) const {
	std::vector<byte> result;

	result.push_back(opcode);
	if (arg)
		result.push_back(*arg);
	if (jump_target) {
		result.push_back(p_jmp_target.value());
	}
	if (string_ref) {
		result.push_back(p_string_ref.value() % 256);
		result.push_back(p_string_ref.value() / 256);
	}
	if (string) {
		ScriptString str(*string);
		const auto strbytes{ str.to_bytes() };
		result.insert(end(result), begin(strbytes), end(strbytes));
	}

	return result;
}

std::size_t script::Instruction::size(void) const {
	std::size_t result{ 1 }; // opcode is 1 byte

	if (arg)
		++result;
	if (jump_target)
		++result;
	if (string_ref)
		result += 2;
	if (string) {
		ScriptString str(*string);
		result += str.to_bytes().size();
	}

	return result;
}
