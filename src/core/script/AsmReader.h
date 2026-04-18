#ifndef SCRIPT_ASMREADER_H
#define SCRIPT_ASMREADER_H

#include "ScriptLayer.h"
#include "./../htConfig.h"

namespace script {

	class AsmReader {


	public:
		AsmReader(void) = default;
		ScriptLayer assemble(const ht::ScriptConfig& config, const std::string& asm_filename) const;
	};

}

#endif
