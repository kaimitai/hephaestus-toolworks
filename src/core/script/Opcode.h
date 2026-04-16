#ifndef SCRIPT_OPCODE_H
#define SCRIPT_OPCODE_H

#include <cstdint>
#include <string>

using byte = uint8_t;

namespace script {

	enum class Flow { Continue, Jump, ConditionalJump, End };
	enum class ArgDomain { None, NPC, Flag };
	enum class ArgType { None, Byte, ImmediateString, PointerString };

	struct Opcode {
		std::string mnemonic;
		Flow flow;
		ArgType argType;
		ArgDomain argDomain;
	};

}

#endif
