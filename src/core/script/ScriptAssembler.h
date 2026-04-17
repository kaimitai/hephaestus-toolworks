#ifndef SCRIPT_ASSEMBLER_H
#define SCRIPT_ASSEMBLER_H

#include "ScriptLayer.h"
#include <cstdint>
#include <vector>

using byte = uint8_t;
using cpu_addr = uint16_t;

namespace script {

	class ScriptAssembler {

		std::vector<std::vector<std::size_t>> build_dense_ptr_table(const ScriptLayer& layer) const;

	public:
		ScriptAssembler(void) = default;
		std::vector<byte> assemble(const ScriptLayer& layer, cpu_addr addr) const;
	};

}

#endif
