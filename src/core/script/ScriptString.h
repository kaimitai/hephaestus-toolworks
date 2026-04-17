#ifndef SCRIPT_STRING_H
#define SCRIPT_STRING_H

#include <array>
#include <cstdint>
#include <string>
#include <string_view>
#include <optional>
#include <vector>

using byte = uint8_t;

namespace script {

	class ScriptString {

		enum class Table {
			Upper,
			Lower,
			Symbols
		};

		std::string m_string;

		// internal symbol tables
		inline static constexpr std::array<std::optional<std::string_view>, 32> UPPER{
			std::nullopt, "A","B","C","D","E","F","G","H","I",
			"J","K","L","M","N","O","P","Q","R","S",
			"T","U","V","W","X","Y","Z",
			std::nullopt, std::nullopt, " ", "<n>", "<p>"
		};

		inline static constexpr std::array<std::optional<std::string_view>, 32> LOWER{
			std::nullopt, "a", "b", "c", "d", "e", "f", "g", "h", "i",
				"j", "k", "l", "m", "n", "o", "p", "q", "r", "s",
				"t", "u", "v", "w", "x", "y", "z",
				std::nullopt, std::nullopt, " ", "<n>", "<p>"
		};

		inline static constexpr std::array<std::optional<std::string_view>, 32> SYMBOLS{
			std::nullopt, "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
				"?", "!", ".", ",", "'", "-",
				std::nullopt, std::nullopt, std::nullopt, std::nullopt,
				std::nullopt, std::nullopt, std::nullopt, std::nullopt,
				std::nullopt, std::nullopt, std::nullopt, std::nullopt,
				" ", "<n>", "<p>"
		};

		static std::optional<byte> find_in_table(const std::array<std::optional<std::string_view>, 32>& tbl,
			std::string_view s);

	public:
		ScriptString(const std::vector<byte>& p_rom, std::size_t p_rom_offset = 0);
		ScriptString(const std::string& p_string);
		std::string get_string(void) const;
		std::vector<byte> to_bytes(void) const;
	};

}

#endif
