#include "ROMParser.h"
#include "boo_constants.h"
#include "./../compression/HuffmanRLE.h"
#include "./../compression/RLE.h"
#include "./../config_constants.h"
#include <stdexcept>
#include <string>
#include <format>

using cpu_addr = uint16_t;

boo::Game boo::ROMParser::parse(const ht::htConfig& p_config,
	const std::vector<byte>& p_rom) const {
	Game game;
	rom::ROM_Manager mgr(c::BANK_COUNT);
	std::size_t world_count{ p_config.count(ht::c::ID_WORLD_COUNT) };

	// parse shared palette
	const auto shared_pal_addr{ p_config.get_address(ht::c::ID_SHARED_BG_PALETTE_ADDR) };
	const auto shared_pal_rom_offset{ mgr.cpu_addr_to_rom_offset(shared_pal_addr.bank, shared_pal_addr.addr) };
	for (std::size_t i{ 0 }; i < 3; ++i)
		game.shared_palette.push_back(p_rom.at(shared_pal_rom_offset + i));

	const auto world_defs_addr{ p_config.get_address(ht::c::ID_WORLD_DEFINITIONS_ADDR) };
	std::size_t world_def_rom_offs{ mgr.cpu_addr_to_rom_offset(world_defs_addr.bank, world_defs_addr.addr) };

	// parse world definitions
	for (std::size_t i{ 0 }; i < world_count; ++i) {
		byte world_def_ptr_idx{ p_rom.at(world_def_rom_offs + i) };
		game.worlds.push_back(parse_world(p_rom, mgr,
			world_def_ptr_idx / 16,
			world_def_ptr_idx % 16));
	}

	// parse world tilesets
	for (std::size_t i{ 0 }; i < world_count; ++i) {
		auto cpu_start{ mgr.read_word(p_rom, 3, 0x8000 + 2 * i) };
		auto rom_offset{ mgr.cpu_addr_to_rom_offset(3, cpu_start) };

		game.worlds[i].tileset = parse_chr_tiles(p_rom, rom_offset,
			c::WORLD_TILESET_CHR_COUNT * 0x10);
	}

	// parse shared tilesets
	std::size_t descr_count{ p_config.count(ht::c::ID_CHR_TRANSFERSCRIPT_COUNT) };
	const auto descr_addr{ p_config.get_address(ht::c::ID_CHR_TRANSFERSCRIPT_ADDR) };
	const auto descr_rom_offset{ mgr.cpu_addr_to_rom_offset(descr_addr.bank, descr_addr.addr) };
	for (std::size_t i{ 0 }; i < descr_count; ++i) {
		byte descriptor{ p_rom.at(descr_rom_offset + i) };
		if (descriptor == 0) {
			game.shared_chr.push_back(HeaderedChr{
				.ppu_index = static_cast<byte>(0),
				.bpp1 = false
				});
		}
		else {
			bool is_1bpp{ i < 3 };
			bool is_compressed{ static_cast<bool>(!(descriptor & 0b10000000)) };
			byte ptr_no{ static_cast<byte>(descriptor & 0x0f) };
			byte bank_no{ static_cast<byte>((descriptor & 0b01110000) >> 4) };

			auto chr_header_start{ mgr.read_word(p_rom, bank_no, 0x8000 + 2 * ptr_no) };
			std::size_t chr_header_rom_offset{ mgr.cpu_addr_to_rom_offset(bank_no, chr_header_start) };

			auto decompressed_byte_size{ mgr.read_word(p_rom, chr_header_rom_offset) };
			if (is_1bpp)
				decompressed_byte_size *= 8;

			auto ppu_idx{ mgr.read_word(p_rom, chr_header_rom_offset + 2) };
			ppu_idx -= 0x1000;
			ppu_idx /= 0x10;

			auto chrtiles{ parse_chr_tiles(p_rom,
				chr_header_rom_offset + 4,
				decompressed_byte_size,
				is_compressed, is_1bpp) };

			game.shared_chr.push_back(HeaderedChr{
				.ppu_index = static_cast<byte>(ppu_idx),
				.bpp1 = false,
				.tiles = chrtiles
				});
		}
	}

	// parse sprite chr banks
	for (std::size_t i{ 0 }; i < p_config.count(ht::c::ID_SPRITE_CHR_BANK_COUNT); ++i) {
		auto addr{ mgr.read_word(p_rom, 4, 0x8000 + 2 * i) };
		auto rom_offset{ mgr.cpu_addr_to_rom_offset(4, addr) };
		auto decomp_size{ mgr.read_word(p_rom, rom_offset) };
		game.sprite_chr.push_back(parse_chr_tiles(p_rom, rom_offset + 2, decomp_size));
	}

	return game;
}

boo::World boo::ROMParser::parse_world(const std::vector<byte>& p_rom,
	const rom::ROM_Manager& p_manager, std::size_t p_bank_no, std::size_t p_ptr_no) const {

	auto ptr_offset{ p_manager.cpu_addr_to_rom_offset(p_bank_no,
		0x8000 + 2 * p_ptr_no)
	};

	auto world_data_cpu_offset{ p_manager.read_word(p_rom, ptr_offset) };
	auto world_data_offset{ p_manager.cpu_addr_to_rom_offset(p_bank_no, world_data_cpu_offset) };

	boo::World world{
		.metatiles = parse_metatiles(p_rom, world_data_offset, c::WORLD_METATILE_COUNT)
	};

	auto world_palette_cpu_offset{ world_data_cpu_offset + 5 * c::WORLD_METATILE_COUNT };

	for (std::size_t i{ 0 }; i < c::WORLD_PALETTE_BYTE_SIZE; i += c::WORLD_PALETTE_BYTE_SIZE / 3) {
		std::vector<byte> wpalette;
		for (std::size_t j{ 0 }; j < 3; ++j)
			wpalette.push_back(p_rom.at(world_palette_cpu_offset + i * 3 + j));
		world.world_palettes.push_back(wpalette);
	}

	auto transition_ptr_cpu_offset{ world_palette_cpu_offset + c::WORLD_PALETTE_BYTE_SIZE };
	auto screen_ptrs_offset{ transition_ptr_cpu_offset + 4 * 2 };
	auto screen_count{
		p_manager.get_ptr_table_size(p_rom, p_bank_no, screen_ptrs_offset)
	};

	if (screen_count % 3 != 0) {
		throw std::runtime_error(
			std::format("Expected ptr table at [{:02x}:{:04x}] to have an entry count which is a multiple of 3", p_bank_no, world_data_cpu_offset)
		);
	}
	else
		screen_count /= 3;

	// read all screen data - 3 ptrs per screen: tilemap, doors/transitions, sprites
	comp::HuffmanRLE huffman;
	for (std::size_t i{ 0 }; i < screen_count; ++i) {
		auto scr_tilemap_ptr_offset{ screen_ptrs_offset + 2 * 3 * i };
		auto screen_tilemap_cpu_offset{ p_manager.read_word(p_rom, p_bank_no, scr_tilemap_ptr_offset) };
		auto scr_command_stream_rom_offset{ p_manager.cpu_addr_to_rom_offset(p_bank_no, screen_tilemap_cpu_offset) };

		Screen screen;

		bool tilemap_found{ false };

		while (!tilemap_found) {
			byte command{ p_rom.at(scr_command_stream_rom_offset++) };
			byte opcode{ static_cast<byte>((command & 0b11000000) >> 6) };
			byte operand{ static_cast<byte>(command & 0b00111111) };

			switch (opcode) {
			case c::SCREEN_DATA_OP_PALETTE_OVERRIDE: {
				screen.palette_overrides.insert({ operand, p_rom.at(scr_command_stream_rom_offset++) });
				break;
			}
			case c::SCREEN_DATA_OP_METATILE_PROPERTY_OVERRIDE: {
				screen.metatile_property_overrides.insert({ operand, p_rom.at(scr_command_stream_rom_offset++) });
				break;
			}
			case c::SCREEN_DATA_OP_JUMP: {
				auto l_cpu_offset_jump{ p_manager.read_word(p_rom, scr_command_stream_rom_offset) };
				scr_command_stream_rom_offset = p_manager.cpu_addr_to_rom_offset(p_bank_no, l_cpu_offset_jump);
				break;
			}

			case c::SCREEN_DATA_OP_TILEMAP_DECOMPRESS: {
				const auto scrtilemapbytes{ huffman.decompress(p_rom, scr_command_stream_rom_offset - 1, c::OVERWORLD_SCREEN_W * c::OVERWORLD_SCREEN_H).bytes };
				for (std::size_t y{ 0 }; y < c::OVERWORLD_SCREEN_H; ++y)
					for (std::size_t x{ 0 }; x < c::OVERWORLD_SCREEN_W; ++x)
						screen.tilemap[y][x] = scrtilemapbytes.at(y * c::OVERWORLD_SCREEN_W + x);

				tilemap_found = true;
				break;
			}
			}
		}

		// parse transitions
		// const auto transition_ptr_cpu_offset

		world.screens.push_back(screen);
	}

	return world;
}

std::vector<boo::Metatile> boo::ROMParser::parse_metatiles(const std::vector<byte>& p_rom,
	std::size_t p_offset, std::size_t p_metatile_count) const {
	std::vector<boo::Metatile> result;

	std::size_t l_property_offset{ p_offset + p_metatile_count * 4 };

	for (std::size_t i{ 0 }; i < p_metatile_count; ++i) {
		std::size_t l_tile_offset{ p_offset + 4 * i };

		result.push_back(boo::Metatile(
			p_rom.at(l_tile_offset), // top left
			p_rom.at(l_tile_offset + 1), // top right
			p_rom.at(l_tile_offset + 2), // bottom left
			p_rom.at(l_tile_offset + 3), // bottom right
			p_rom.at(l_property_offset + i)
		));
	}

	return result;
}

std::vector<nes::ChrTile> boo::ROMParser::parse_chr_tiles(const std::vector<byte>& p_rom,
	std::size_t p_rom_offset, std::size_t p_decompressed_byte_size,
	bool p_compressed, bool p_1bpp) const {
	std::vector<byte> chrbytes;
	std::size_t tile_count{ p_1bpp ? p_decompressed_byte_size / 0x08 :
		p_decompressed_byte_size / 0x10 };

	if (p_compressed) {
		comp::RLE rle;
		chrbytes = rle.decompress(p_rom, p_decompressed_byte_size, p_rom_offset);
	}
	else
		chrbytes = std::vector<byte>(begin(p_rom) + p_rom_offset,
			begin(p_rom) + p_rom_offset + p_decompressed_byte_size);

	if (p_1bpp) {
		std::vector<byte> chrdoubled;
		for (std::size_t i{ 0 }; i < chrbytes.size(); i += 8) {
			for (std::size_t k{ 0 }; k < 2; ++k)
				for (std::size_t j{ 0 }; j < 8; ++j)
					chrdoubled.push_back(chrbytes.at(i + j));
		}
		chrbytes = std::move(chrdoubled);
	}

	std::vector<nes::ChrTile> result;

	for (std::size_t i{ 0 }; i < tile_count; ++i)
		result.push_back(nes::ChrTile(chrbytes, i * 0x10));

	return result;
}
