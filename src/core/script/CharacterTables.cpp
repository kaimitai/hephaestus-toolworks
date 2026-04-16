#include "CharacterTables.h"
#include <stdexcept>

void script::CharacterTables::validate(void) const {
	if (upper.size() != 32 || lower.size() != 32 || symbols.size() != 32)
		throw std::runtime_error("Character tables must have size 32");
}

void script::CharacterTables::initialize_default(void) {
	upper = {
		std::nullopt, "A", "B", "C", "D", "E", "F", "G", "H", "I",
		"J", "K", "L", "M", "N", "O", "P", "Q", "R", "S",
		"T", "U", "V", "W", "X", "Y", "Z", std::nullopt, std::nullopt, " ",
		"<n>", "<p>"
	};

	lower = {
	std::nullopt, "a", "b", "c", "d", "e", "f", "g", "h", "i",
	"j", "k", "l", "m", "n", "o", "p", "q", "r", "s",
	"t", "u", "v", "w", "x", "y", "z", std::nullopt, std::nullopt, " ",
	"<n>", "<p>"
	};

	symbols = {
		std::nullopt, "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
		"?", "!", ".", ",", "'", "-"
	};

	while (symbols.size() < 32)
		symbols.push_back(std::nullopt);

	symbols[0x1d] = " ";
	symbols[0x1e] = "<n>";
	symbols[0x1f] = "<p>";
}
