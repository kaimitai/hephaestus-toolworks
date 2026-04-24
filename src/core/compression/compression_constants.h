#ifndef COMPRESSION_CONSTANTS_H
#define COMPRESSION_CONSTANTS_H

#include <cstdint>

using byte = uint8_t;

namespace comp {

	namespace c {

		constexpr byte RLE_SIGNAL_BYTE{ 0x81 };

	}

}

#endif
