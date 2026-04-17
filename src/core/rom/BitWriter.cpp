#include "BitWriter.h"

rom::BitWriter::BitWriter() :
	current_byte{ 0 },
	bits_filled{ 0 }
{
}

void rom::BitWriter::write_bits(uint32_t value, int n) {
	for (int i = n - 1; i >= 0; --i) {
		current_byte <<= 1;
		current_byte |= (value >> i) & 1;

		++bits_filled;

		if (bits_filled == 8) {
			data.push_back(current_byte);
			current_byte = 0;
			bits_filled = 0;
		}
	}
}

void rom::BitWriter::flush() {
	if (bits_filled > 0) {
		current_byte <<= (8 - bits_filled);
		data.push_back(current_byte);
		current_byte = 0;
		bits_filled = 0;
	}
}

const std::vector<byte>& rom::BitWriter::get_data() const {
	return data;
}
