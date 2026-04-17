#include "ScriptString.h"
#include "./../rom/BitReader.h"
#include "./../rom/BitWriter.h"
#include <format>
#include <stdexcept>
#include <unordered_map>

script::ScriptString::ScriptString(const std::vector<byte>& p_rom, std::size_t p_rom_offset) {
	rom::BitReader reader(p_rom_offset + 1);
	uint8_t tokens_left{ p_rom.at(p_rom_offset) };

	Table base_table{ Table::Upper };
	bool numbers_mode{ false };

	const auto get_table = [&](void) -> const auto& {
		if (numbers_mode)
			return SYMBOLS;
		else
			return (base_table == Table::Upper ? UPPER : LOWER);
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

script::ScriptString::ScriptString(const std::string& p_string) :
	m_string{ p_string }
{
}

std::string script::ScriptString::get_string(void) const {
	return m_string;
}

std::vector<byte> script::ScriptString::to_bytes(void) const {
	std::vector<byte> result, tokens;

	Table base{ Table::Upper };
	bool numbers{ false };

	auto emit = [&](byte t) {
		tokens.push_back(t);
		};

	auto toggle_numbers = [&]() {
		emit(0x00);
		numbers = !numbers;
		};

	auto ensure_case = [&](Table wanted) {
		if (base != wanted) {
			emit(0x1B);
			base = wanted;
		}
		};

	for (size_t i = 0; i < m_string.size();) {

		// --- multi-char tokens ---
		if (m_string[i] == '<') {
			size_t end = m_string.find('>', i);
			if (end == std::string::npos)
				throw std::runtime_error("Unterminated token");

			std::string token = m_string.substr(i, end - i + 1);

			if (token == "<hero>") {
				emit(0x1C); emit(0x00);
			}
			else if (token == "<heroine>") {
				emit(0x1C); emit(0x01);
			}
			else if (token == "<p>") {
				emit(0x1F);
				base = Table::Upper;
				numbers = false;
			}
			else if (token == "<n>") {
				emit(0x1E);
			}
			else {
				throw std::runtime_error("Unknown token: " + token);
			}

			i = end + 1;
			continue;
		}

		std::string_view ch(&m_string[i], 1);

		// --- numbers mode ---
		if (numbers) {
			if (auto tok = find_in_table(SYMBOLS, ch)) {
				emit(*tok);

				if (*tok >= 0x0B)
					numbers = false;

				++i;
				continue;
			}

			// can't encode → exit numbers mode
			toggle_numbers();
		}

		// --- try current table ---
		const auto& tbl = (base == Table::Upper ? UPPER : LOWER);

		if (auto tok = find_in_table(tbl, ch)) {
			emit(*tok);
			++i;
			continue;
		}

		// --- try other case ---
		const auto& other = (base == Table::Upper ? LOWER : UPPER);

		if (auto tok = find_in_table(other, ch)) {
			ensure_case(base == Table::Upper ? Table::Lower : Table::Upper);
			emit(*tok);
			++i;
			continue;
		}

		// --- fallback to symbols ---
		if (auto tok = find_in_table(SYMBOLS, ch)) {
			toggle_numbers();
			emit(*tok);

			if (*tok >= 0x0B)
				numbers = false;

			++i;
			continue;
		}

		throw std::runtime_error(
			std::format("Cannot encode character '{}' in script-string", ch)
		);
	}

	if (tokens.size() > 255)
		throw std::runtime_error(
			std::format("Too many tokens in string (was {}, but maximum is 255)", tokens.size())
		);

	// --- bit packing ---
	rom::BitWriter writer;

	for (byte t : tokens)
		writer.write_bits(t, 5);

	writer.flush();

	result.push_back(static_cast<byte>(tokens.size()));

	const auto& packed = writer.get_data();
	result.insert(result.end(), packed.begin(), packed.end());

	return result;
}

std::optional<byte> script::ScriptString::find_in_table(
	const std::array<std::optional<std::string_view>, 32>& tbl, std::string_view s) {
	for (size_t i = 0; i < tbl.size(); ++i) {
		if (tbl[i] && *tbl[i] == s)
			return static_cast<byte>(i);
	}
	return std::nullopt;
}
