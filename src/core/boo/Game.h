#ifndef BOO_GAME_H
#define BOO_GAME_H

#include <vector>
#include "World.h"

namespace boo {

	struct Game {

		std::vector<World> worlds;

		Game(void) = default;
	};

}

#endif
