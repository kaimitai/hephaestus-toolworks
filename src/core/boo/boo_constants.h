#ifndef BOO_CONSTANTS_H
#define BOO_CONSTANTS_H

#include <cstddef>
#include <cstdint>

using byte = uint8_t;

namespace boo {

	namespace c {

		constexpr std::size_t OVERWORLD_SCREEN_W{ 48 };
		constexpr std::size_t OVERWORLD_SCREEN_H{ 11 };
		constexpr std::size_t WORLD_METATILE_COUNT{ 64 };
		constexpr std::size_t WORLD_PALETTE_BYTE_SIZE{ 9 };
		constexpr std::size_t BANK_COUNT{ 8 };

		// screen decode VM
		constexpr byte SCREEN_DATA_OP_TILEMAP_DECOMPRESS{ 0b00 };
		constexpr byte SCREEN_DATA_OP_PALETTE_OVERRIDE{ 0b01 };
		constexpr byte SCREEN_DATA_OP_METATILE_PROPERTY_OVERRIDE{ 0b10 };
		constexpr byte SCREEN_DATA_OP_JUMP{ 0b11 };
	}

}

#endif
