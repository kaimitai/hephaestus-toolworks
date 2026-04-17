#ifndef SCRIPT_INSTRUCTION_H
#define SCRIPT_INSTRUCTION_H

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

using byte = uint8_t;
using cpu_addr = uint16_t;

namespace script {

	struct Instruction {
		byte opcode = 0xff;

		std::optional<byte> arg;
		std::optional<std::size_t> jump_target;
		std::optional<std::size_t> string_ref;
		std::optional<std::string> string;
		std::optional<std::size_t> byte_offset;

		std::vector<byte> to_bytes(std::optional<byte> p_jmp_target,
			std::optional<cpu_addr> p_string_ref) const;
		std::size_t size(void) const;
	};

}

#endif
