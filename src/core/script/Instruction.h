#ifndef SCRIPT_INSTRUCTION_H
#define SCRIPT_INSTRUCTION_H

#include <cstdint>
#include <optional>
#include <string>

using byte = uint8_t;

namespace script {

	struct Instruction {
		byte opcode;

		std::optional<byte> arg;
		std::optional<std::size_t> jump_target;
		std::optional<std::string> string;
	};

}

#endif
