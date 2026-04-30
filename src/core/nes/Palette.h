#ifndef NES_PALETTE_H
#define NES_PALETTE_H

#include <cstdint>
#include <vector>

using byte = unsigned char;

namespace nes {

	struct Palette {

		std::vector<std::vector<byte>> colors;

		std::vector<byte> get_flat_palette(void) const;
	};

}

#endif
