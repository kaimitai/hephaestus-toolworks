#include "RLE.h"
#include "compression_constants.h"
#include <stdexcept>

std::vector<byte> comp::RLE::decompress(const std::vector<byte>& p_bytes,
	std::size_t p_expected_size, std::size_t p_offset) const {
	std::vector<byte> out;
	out.reserve(p_expected_size);

	size_t i{ p_offset };
	byte last{ 0 };
	bool have_last = false;

	while (out.size() < p_expected_size) {
		if (i >= p_bytes.size())
			throw std::runtime_error("RLE decode error: input ended before expected output size was reached.");

		byte b{ p_bytes[i++] };

		if (b == c::RLE_SIGNAL_BYTE) {
			if (i >= p_bytes.size())
				throw std::runtime_error("RLE decode error: signal byte at end of stream.");

			byte count{ p_bytes[i++] };

			if (count == 0) {
				out.push_back(c::RLE_SIGNAL_BYTE);
				last = c::RLE_SIGNAL_BYTE;
				have_last = true;
			}
			else {
				if (!have_last)
					throw std::runtime_error("RLE decode error: repeat with no previous byte.");

				for (int k{ 1 }; k < count && out.size() < p_expected_size; ++k)
					out.push_back(last);
			}
		}
		else {
			out.push_back(b);
			last = b;
			have_last = true;
		}
	}

	return out;
}

std::vector<byte> comp::RLE::compress(const std::vector<byte>& p_bytes) const {
	std::vector<byte> out;
	if (p_bytes.empty())
		return out;

	auto emit_literal = [&](byte b) {
		if (b == c::RLE_SIGNAL_BYTE) {
			out.push_back(c::RLE_SIGNAL_BYTE);
			out.push_back(0x00);
		}
		else {
			out.push_back(b);
		}
		};

	size_t i = 0;
	while (i < p_bytes.size()) {
		byte b{ p_bytes[i] };
		emit_literal(b);
		++i;

		size_t run{ 0 };
		while (i < p_bytes.size() && p_bytes[i] == b && run < 255) {
			++run;
			++i;
		}

		if (run >= 3) {
			out.push_back(c::RLE_SIGNAL_BYTE);
			out.push_back(static_cast<byte>(run));
		}
		else {
			for (size_t k{ 0 }; k < run; ++k)
				emit_literal(b);
		}
	}

	return out;
}
