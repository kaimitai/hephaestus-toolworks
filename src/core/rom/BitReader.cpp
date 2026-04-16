#include "BitReader.h"

rom::BitReader::BitReader(std::size_t p_offset) :
	byte_offset{ p_offset },
	current_byte{ 0 },
	bits_left{ 0 }
{
}

uint32_t rom::BitReader::read_bits(const std::vector<byte>& p_rom, int n) {
	uint32_t result{ 0 };

	while (n--) {
		if (bits_left == 0) {
			current_byte = p_rom.at(byte_offset++);
			bits_left = 8;
		}

		result <<= 1;
		result |= (current_byte >> 7) & 1;

		current_byte <<= 1;
		--bits_left;
	}

	return result;
}

std::size_t rom::BitReader::byte_pos(void) const {
	return byte_offset;
}
