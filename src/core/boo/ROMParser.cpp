#include "ROMParser.h"
#include "boo_constants.h"
#include "./../compression/HuffmanRLE.h"
#include <stdexcept>
#include <string>
#include <format>

using cpu_addr = uint16_t;

boo::World boo::ROMParser::parse(const std::vector<byte>& p_rom,
	std::size_t p_bank_no, std::size_t p_ptr_no) const {
	rom::ROM_Manager mgr(c::BANK_COUNT);

	return parse_world(p_rom, mgr, p_bank_no, p_ptr_no);
}

boo::World boo::ROMParser::parse_world(const std::vector<byte>& p_rom,
	const rom::ROM_Manager& p_manager, std::size_t p_bank_no, std::size_t p_ptr_no) const {

	auto ptr_offset{ p_manager.cpu_addr_to_rom_offset(p_bank_no,
		0x8000 + 2 * p_ptr_no)
	};

	auto world_data_cpu_offset{ p_manager.read_word(p_rom, ptr_offset) };
	auto world_data_offset{ p_manager.cpu_addr_to_rom_offset(p_bank_no, world_data_cpu_offset) };

	boo::World world{
		.metatiles = parse_metatiles(p_rom, world_data_offset)
	};

	auto transition_ptr_cpu_offset{ world_data_cpu_offset + 5 * c::WORLD_METATILE_COUNT + 9 };
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

std::array<boo::Metatile, boo::c::WORLD_METATILE_COUNT> boo::ROMParser::parse_metatiles(const std::vector<byte>& p_rom,
	std::size_t p_offset) const {
	std::array<Metatile, c::WORLD_METATILE_COUNT> result;

	std::size_t l_property_offset{ p_offset + c::WORLD_METATILE_COUNT * 4 };

	for (std::size_t i{ 0 }; i < c::WORLD_METATILE_COUNT; ++i) {
		std::size_t l_tile_offset{ p_offset + 4 * i };

		result[i] = boo::Metatile(
			p_rom.at(l_tile_offset), // top left
			p_rom.at(l_tile_offset + 1), // top right
			p_rom.at(l_tile_offset + 2), // bottom left
			p_rom.at(l_tile_offset + 3), // bottom right
			p_rom.at(l_property_offset + i)
		);
	}

	return result;
}
