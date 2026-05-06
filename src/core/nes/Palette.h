#ifndef NES_PALETTE_H
#define NES_PALETTE_H

#include <cstdint>
#include <vector>

using byte = unsigned char;

namespace nes {

	struct Palette {

		std::vector<std::vector<byte>> colors;

		byte get_nes_color(std::size_t p_sub_palette, std::size_t p_color_no) const;
		std::vector<byte> get_flat_palette(void) const;
	};

}

#endif
