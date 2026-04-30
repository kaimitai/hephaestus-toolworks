#ifndef NES_CHR_TILE_H
#define NES_CHR_TILE_H

#include <cstdint>
#include <vector>

using byte = uint8_t;

namespace nes {

	struct CanonChoice {
		bool h = false;
		bool v = false;
	};

	class ChrTile {

		std::vector<std::vector<byte>> m_tile_data;

	public:
		ChrTile(const std::vector<byte>& p_rom_data, std::size_t p_offset = 0);
		ChrTile(void);
		void flip_h(void);
		void flip_v(void);
		void flip(bool h, bool v);
		bool is_empty(void) const;
		CanonChoice canonicalize(void); // flips *this* to canonical form and returns flips applied

		bool operator<(const nes::ChrTile& rhs) const;
		bool operator==(const nes::ChrTile& rhs) const;
		std::vector<byte> to_bytes(void) const;

		std::size_t w(void) const;
		std::size_t h(void) const;
		byte get_color(std::size_t p_x, std::size_t p_y) const;

		void set_color(std::size_t p_x, std::size_t p_y, byte p_pal_idx);
	};

}

#endif
