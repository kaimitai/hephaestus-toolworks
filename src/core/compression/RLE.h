#ifndef COMP_RLE_H
#define COMP_RLE_H

#include <cstdint>
#include <vector>

using byte = uint8_t;

namespace comp {

	class RLE {

	public:
		RLE(void) = default;

		std::vector<byte> decompress(const std::vector<byte>& p_bytes, std::size_t p_expected_size) const;
		std::vector<byte> compress(const std::vector<byte>& p_bytes) const;
	};

}

#endif
