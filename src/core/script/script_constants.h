#ifndef SCRIPT_CONTANTS_H
#define SCRIPT_CONTANTS_H

#include <string_view>

namespace script {

	namespace c {

		constexpr std::string_view ASM_DIRECTIVE_ENTRYPOINT{ ".entrypoint" };

		constexpr std::string_view ASM_SECTION_DEFINES{ "[defines]" };
		constexpr std::string_view ASM_SECTION_SCRIPT{ "[script]" };
	}

}

#endif
