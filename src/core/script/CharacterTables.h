#ifndef SCRIPT_CHARACTER_TABLES_H
#define SCRIPT_CHARACTER_TABLES_H

#include <vector>
#include <optional>
#include <string>

namespace script {

	struct CharacterTables {

		std::vector<std::optional<std::string>> upper;
		std::vector<std::optional<std::string>> lower;
		std::vector<std::optional<std::string>> symbols;

		void validate(void) const;
		void initialize_default(void);
	};

}

#endif
