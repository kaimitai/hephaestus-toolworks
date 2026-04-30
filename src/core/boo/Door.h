#ifndef BOO_DOOR_H
#define BOO_DOOR_H

#include <cstdint>

using byte = uint8_t;

namespace boo {

	struct Door {
		byte x, y, room_dest, return_dest;
	};

}

#endif
