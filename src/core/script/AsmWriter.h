#ifndef SCRIPT_ASMWRITER_H
#define SCRIPT_ASMWRITER_H

#include "./../htConfig.h"
#include "AsmView.h"
#include <string>

namespace script {

	class AsmWriter {

	public:
		AsmWriter(void) = default;
		void write_asm_file(const ht::ScriptConfig& config, const AsmView& view,
			const std::string& out_filename) const;
	};

}

#endif
