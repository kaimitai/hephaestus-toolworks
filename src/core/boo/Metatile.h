#ifndef BOO_METATILE_H
#define BOO_METATILE_H

#include <array>
#include <cstdint>

using byte = uint8_t;

namespace boo {

	struct Metatile {

		byte palette_no; // 0-3
		byte properties; // 6-bit property field

		std::array<byte, 4> tilemap;

		Metatile(byte p_tl, byte p_tr, byte p_bl, byte p_br, byte p_packed_properties);
		Metatile(void);
	};

}

#endif
