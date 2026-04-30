#include "HuffmanRLE.h"
#include <array>
#include <stdexcept>

byte comp::HuffmanRLE::read_symbol(
	rom::BitReader& reader,
	const std::vector<byte>& bytes,
	std::size_t table_offset,
	std::size_t bitstream_offset) const
{
	static constexpr std::array<byte, 5> OFFSETS{
		0x00,
		0x03,
		0x0A,
		0x19,
		0x38
	};

	int bits_to_read = 2;
	int table_group = 0;
	byte sentinel = 3;

	while (true) {
		byte value = static_cast<byte>(
			reader.read_bits(bytes, bits_to_read)
			);

		if (value != sentinel) {
			std::size_t index =
				table_offset +
				OFFSETS.at(table_group) +
				value;

			if (index >= bitstream_offset)
				throw std::runtime_error(
					"Huffman symbol index spilled into bitstream."
				);

			return bytes.at(index);
		}

		++bits_to_read;
		++table_group;

		if (table_group >= static_cast<int>(OFFSETS.size()))
			throw std::runtime_error("Invalid Huffman symbol");

		sentinel = static_cast<byte>(
			(sentinel << 1) | 1
			);
	}
}

comp::HRLEDecompressionResult comp::HuffmanRLE::decompress(
	const std::vector<byte>& bytes,
	std::size_t offset,
	std::size_t expected_size) const
{
	const byte bitstream_rel = bytes.at(offset);

	const std::size_t table_offset = offset + 1;
	const std::size_t bitstream_offset =
		table_offset + bitstream_rel;

	rom::BitReader reader{ bitstream_offset };

	std::vector<byte> result;
	result.reserve(expected_size);

	byte previous_literal = 0;
	bool expecting_repeat = false;

	while (result.size() < expected_size) {
		const byte symbol =
			read_symbol(reader, bytes, table_offset, bitstream_offset);

		if (expecting_repeat) {
			if (symbol == 0x00) {
				// escaped literal 0x81
				result.push_back(0x81);
			}
			else {
				for (int i = 0; i < symbol; ++i)
					result.push_back(previous_literal);
			}

			expecting_repeat = false;
		}
		else if (symbol & 0x80) {
			// actual RLE marker
			expecting_repeat = true;
		}
		else {
			previous_literal = symbol;
			result.push_back(symbol);
		}
	}

	return HRLEDecompressionResult{
		.bytes = result,
		.offset_at_end = reader.byte_pos()
	};
}
