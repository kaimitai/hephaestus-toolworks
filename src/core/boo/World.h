#ifndef BOO_WORLD_H
#define BOO_WORLD_H

#include <cstdint>
#include <vector>
#include "boo_constants.h"
#include "Metatile.h"
#include "Screen.h"
#include "./../nes/ChrTile.h"

using byte = uint8_t;
using word = uint16_t;

namespace boo {

	struct World {

		std::vector<Metatile> metatiles;
		std::vector<Screen> screens;
		std::vector<nes::ChrTile> tileset;
		// 3x3 palette indexes, bg col is $0f and the last sub-palette is shared at the game-level
		std::vector<std::vector<byte>> world_palettes;

		// door destinations
		std::vector<byte> door_x, door_y, door_dest, door_ret;
	};

}

#endif
