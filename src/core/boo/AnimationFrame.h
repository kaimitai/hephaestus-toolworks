#ifndef BOO_ANIMATION_FRAME_H
#define BOO_ANIMATION_FRAME_H

#include <cstdint>
#include <optional>
#include <vector>

using byte = uint8_t;

namespace boo {

	struct Tile {
		byte idx, pal;
		bool v_flip, h_flip;
	};

	struct AnimationFrame {
		std::vector<std::vector<std::optional<Tile>>> tilemap;

		void initialize(std::size_t w, std::size_t h);
		void append_column(const std::vector<byte>& p_rom, std::size_t p_offset,
			byte p_column_height, std::size_t p_y_offset = 0);

		std::size_t w(void) const;
		std::size_t h(void) const;
	};

}

#endif
