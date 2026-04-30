#ifndef BOO_SCREEN_H
#define BOO_SCREEN_H

#include <array>
#include <cstdint>
#include <unordered_map>
#include <vector>
#include "boo_constants.h"
#include "Door.h"

using byte = uint8_t;

namespace boo {

	struct Screen {

		// screen-level overrides
		std::unordered_map<byte, byte> palette_overrides,
			metatile_property_overrides;

		// vector <sprite>
		std::vector <Door> doors;
		byte scroll_left, scroll_right,
			minimap_x, minimap_y;
		std::array<std::array<byte, c::OVERWORLD_SCREEN_W>, c::OVERWORLD_SCREEN_H> tilemap;

		Screen(void);
	};

}

#endif
