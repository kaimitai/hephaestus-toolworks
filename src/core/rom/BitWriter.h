#ifndef ROM_BITWRITER_H
#define ROM_BITWRITER_H

#include <cstdint>
#include <vector>

using byte = uint8_t;

namespace rom {

	class BitWriter {

		std::vector<byte> data;
		byte current_byte;
		int bits_filled;

	public:
		BitWriter();

		void write_bits(uint32_t value, int n);
		void flush();

		const std::vector<byte>& get_data() const;
	};

}

#endif
