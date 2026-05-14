#include "Game.h"

std::vector<nes::ChrTile> boo::Game::get_world_tileset(std::size_t p_world_no) const {
	std::vector<nes::ChrTile> result{ worlds.at(p_world_no).tileset };
	while (result.size() < 256)
		result.push_back(nes::ChrTile());

	const auto add_tileset = [](std::vector<nes::ChrTile>& out, const HeaderedChr& in) -> void {
		for (std::size_t i{ 0 }; i < in.tiles.size(); ++i)
			out[i + in.ppu_index] = in.tiles[i];
		};

	add_tileset(result, shared_chr.at(1));
	add_tileset(result, shared_chr.at(4));
	add_tileset(result, shared_chr.at(5));
	add_tileset(result, shared_chr.at(
		p_world_no >= 8 ? 14 :
		12
	));

	return result;
}

nes::Palette boo::Game::get_screen_bg_palette(std::size_t p_world_no, std::size_t p_screen_no) const {
	nes::Palette result;

	for (std::size_t i{ 0 }; i < 3; ++i) {
		std::vector<byte> subpal{ c::SHARED_BG_PALETTE_COLOR };
		for (byte b : worlds.at(p_world_no).world_palettes.at(i))
			subpal.push_back(b);
		result.colors.push_back(subpal);
	}

	std::vector<byte> subpal{ c::SHARED_BG_PALETTE_COLOR };
	for (byte b : shared_palette)
		subpal.push_back(b);

	result.colors.push_back(subpal);

	for (const auto& kv : worlds[p_world_no].screens.at(p_screen_no).palette_overrides)
		result.colors.at(kv.first / 4).at(kv.first % 4) = kv.second;

	return result;
}

nes::Palette boo::Game::get_screen_sprite_palette(std::size_t p_world_no, std::size_t p_screen_no) const {
	nes::Palette result;

	const auto& screen{ worlds.at(p_world_no).screens.at(p_screen_no) };

	std::vector<byte> subpal0{ c::SHARED_BG_PALETTE_COLOR };
	subpal0.insert(end(subpal0), begin(sprite_palette_pool) + screen.sprite_pal0_offset,
		begin(sprite_palette_pool) + screen.sprite_pal0_offset + 3);
	result.colors.push_back(subpal0);

	std::vector<byte> subpal1{ c::SHARED_BG_PALETTE_COLOR };
	subpal1.insert(end(subpal1), begin(sprite_palette_pool) + screen.sprite_pal1_offset,
		begin(sprite_palette_pool) + screen.sprite_pal1_offset + 3);
	result.colors.push_back(subpal1);

	for (std::size_t i{ 0 }; i < 2; ++i) {
		std::vector<byte> subpal{ c::SHARED_BG_PALETTE_COLOR };
		for (byte b : shared_sprite_palettes.at(i))
			subpal.push_back(b);
		result.colors.push_back(subpal);
	}

	return result;
}
