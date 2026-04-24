#ifndef COMP_HUFFMAN_RLE_H
#define COMP_HUFFMAN_RLE_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include "./../rom/BitReader.h"

using byte = uint8_t;

namespace comp {

	class HuffmanRLE {

		byte read_symbol(rom::BitReader& reader, const std::vector<byte>& data,
			std::size_t table_offset, std::size_t bitstream_offset) const;

	public:
		std::vector<byte> decompress(const std::vector<byte>& p_bytes,
			std::size_t p_offset, std::size_t p_expected_decompressed_size) const;
	};

}

#endif
