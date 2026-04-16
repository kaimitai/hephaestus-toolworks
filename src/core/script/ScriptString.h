#ifndef SCRIPT_STRING_H
#define SCRIPT_STRING_H

#include <cstdint>
#include <string>
#include <vector>
#include "CharacterTables.h"

using byte = uint8_t;

namespace script {

	class ScriptString {

		enum class Table {
			Upper,
			Lower,
			Symbols
		};

		std::string m_string;

	public:
		ScriptString(const std::vector<byte>& p_rom, std::size_t p_rom_offset,
			const CharacterTables& tables);
		std::string get_string(void) const;
	};

}

#endif
