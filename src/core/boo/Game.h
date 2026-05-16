#ifndef BOO_GAME_H
#define BOO_GAME_H

#include <unordered_map>
#include <vector>
#include "./../nes/ChrTile.h"
#include "./../nes/Palette.h"
#include "World.h"
#include "AnimationFrame.h"

namespace boo {

	struct HeaderedChr {
		byte ppu_index;
		bool bpp1;
		std::vector<nes::ChrTile> tiles;
	};

	struct Game {

		std::vector<World> worlds;
		std::vector<byte> shared_palette, sprite_palette_pool;
		std::vector<std::vector<byte>> shared_sprite_palettes;
		std::vector<HeaderedChr> shared_chr;
		std::vector<std::vector<nes::ChrTile>> sprite_chr;
		std::unordered_map<byte, std::vector<AnimationFrame>> sprite_animations;

		Game(void) = default;

		nes::Palette get_screen_bg_palette(std::size_t p_world_no, std::size_t p_screen_no,
			bool apply_overrides = true) const;
		nes::Palette get_screen_sprite_palette(std::size_t p_world_no, std::size_t p_screen_no) const;
		std::vector<nes::ChrTile> get_world_tileset(std::size_t p_world_no) const;
	};

}

#endif
