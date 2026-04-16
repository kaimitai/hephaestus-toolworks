#include "ScriptString.h"
#include "./../rom/BitReader.h"
#include <format>
#include <stdexcept>

script::ScriptString::ScriptString(const std::vector<byte>& p_rom, std::size_t p_rom_offset,
	const CharacterTables& tables) {
	tables.validate();

	rom::BitReader reader(p_rom_offset + 1);
	uint8_t tokens_left{ p_rom.at(p_rom_offset) };

	Table base_table{ Table::Upper };
	bool numbers_mode{ false };

	const auto get_table = [&](void) -> const std::vector<std::optional<std::string>>&{
		if (numbers_mode)
			return tables.symbols;
		else
			return (base_table == Table::Upper ? tables.upper : tables.lower);
		};

	while (tokens_left > 0) {
		uint8_t t = static_cast<uint8_t>(reader.read_bits(p_rom, 5));
		--tokens_left;

		if (t == 0b00000) {
			numbers_mode = !numbers_mode;
			continue;
		}
		else if (t == 0b11111) {
			m_string += "<p>";
			base_table = Table::Upper;
			numbers_mode = false;
			continue;
		}
		else if (t == 0b11011) {
			base_table = (base_table == Table::Upper ? Table::Lower : Table::Upper);
			continue;
		}
		else if (t == 0b11100) {
			uint8_t sub = static_cast<uint8_t>(reader.read_bits(p_rom, 5));
			--tokens_left;

			if (sub == 0)
				m_string += "<hero>";
			else if (sub == 1)
				m_string += "<heroine>";
			else
				throw std::runtime_error(
					std::format("Unknown name token: {:05b}", sub)
				);

			continue;
		}

		const auto& tbl = get_table();
		if (t >= tbl.size() || !tbl[t])
			throw std::runtime_error(std::format("Unhandled token: {:05b}", t));

		m_string += *tbl[t];

		// In numbers/symbol mode, digits 0-9 keep the mode active.
		// Punctuation/symbols >= 0x0B automatically exit it.
		if (numbers_mode && t >= 0x0B)
			numbers_mode = false;
	}
}

std::string script::ScriptString::get_string(void) const {
	return m_string;
}
