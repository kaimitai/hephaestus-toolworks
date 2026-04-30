#ifndef BOO_WORLD_H
#define BOO_WORLD_H

#include <array>
#include <cstdint>
#include <vector>
#include "boo_constants.h"
#include "Metatile.h"
#include "Screen.h"

using byte = uint8_t;
using word = uint16_t;

namespace boo {

	struct World {

		std::array<Metatile, c::WORLD_METATILE_COUNT> metatiles;
		std::vector<Screen> screens;

	};

}

#endif
