#ifndef ROM_BITREADER_H
#define ROM_BITREADER_H

#include <cstdint>
#include <vector>

using byte = uint8_t;

namespace rom {

	class BitReader {

		std::size_t byte_offset;
		byte current_byte;
		int bits_left;

	public:
		BitReader(std::size_t p_offset);
		uint32_t read_bits(const std::vector<byte>& p_rom, int n);
		std::size_t byte_pos(void) const;
	};

}

#endif
