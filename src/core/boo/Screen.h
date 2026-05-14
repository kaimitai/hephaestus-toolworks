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

	struct Sprite {
		byte id, x_tile, y_pixel;
	};

	struct Screen {

		// screen-level overrides
		std::unordered_map<byte, byte> palette_overrides,
			metatile_property_overrides;

		std::vector<Sprite> sprites;
		// std::vector <Door> doors;
		std::vector<byte> door_idxs;
		byte scroll_left, scroll_right,
			minimap_x, minimap_y,
			sprite_pal0_offset, sprite_pal1_offset;
		std::array<std::array<byte, c::OVERWORLD_SCREEN_W>, c::OVERWORLD_SCREEN_H> tilemap;

		Screen(void);
	};

}

#endif
