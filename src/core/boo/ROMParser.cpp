#include "ROMParser.h"
#include "boo_constants.h"
#include "./../compression/HuffmanRLE.h"
#include "./../compression/RLE.h"
#include "./../config_constants.h"
#include <array>
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
	const auto shared_pal_addr{ p_config.address(ht::c::ID_SHARED_BG_PALETTE_ADDR) };
	const auto shared_pal_rom_offset{ mgr.cpu_addr_to_rom_offset(shared_pal_addr.bank, shared_pal_addr.addr) };
	for (std::size_t i{ 0 }; i < 3; ++i)
		game.shared_palette.push_back(p_rom.at(shared_pal_rom_offset + i));

	// parse shared sprite palettes
	const auto shared_spr_pal_addr{ p_config.address(ht::c::ID_SHARED_SPRITE_PALETTE_ADDR) };
	const auto shared_spr_pal_rom_offset{ mgr.cpu_addr_to_rom_offset(shared_spr_pal_addr.bank, shared_spr_pal_addr.addr) };
	game.shared_sprite_palettes.push_back(mgr.read_bytes(p_rom, shared_spr_pal_rom_offset, 3));
	game.shared_sprite_palettes.push_back(mgr.read_bytes(p_rom, shared_spr_pal_rom_offset + 3, 3));

	// parse sprite palette pool
	const auto spr_pal_pool_addr{ p_config.address(ht::c::ID_SPRITE_PALETTE_POOL) };
	const auto spr_pal_pool_rom_offset{ mgr.cpu_addr_to_rom_offset(spr_pal_pool_addr.bank, spr_pal_pool_addr.addr) };
	game.sprite_palette_pool = mgr.read_bytes(p_rom, spr_pal_pool_rom_offset,
		c::SPRITE_PALETTE_SHARED_SIZE);

	const auto world_defs_addr{ p_config.address(ht::c::ID_WORLD_DEFINITIONS_ADDR) };
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
	const auto descr_addr{ p_config.address(ht::c::ID_CHR_TRANSFERSCRIPT_ADDR) };
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

	// parse sprite animation frames
	const auto& animconfig{ p_config.get_animation_config() };
	for (const auto& kv : animconfig)
		game.sprite_animations.insert(
			std::make_pair(kv.first,
				parse_animation_frames(p_rom, mgr, kv.first, kv.second)
			)
		);

	parse_sprite_palette_overrides(p_rom, mgr, game);
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
	auto world_palette_rom_offset{ p_manager.cpu_addr_to_rom_offset(p_bank_no, world_palette_cpu_offset) };

	for (std::size_t i{ 0 }; i < c::WORLD_PALETTE_BYTE_SIZE; i += c::WORLD_PALETTE_BYTE_SIZE / 3) {
		std::vector<byte> wpalette;
		for (std::size_t j{ 0 }; j < 3; ++j)
			wpalette.push_back(p_rom.at(world_palette_rom_offset + i + j));
		world.world_palettes.push_back(wpalette);
	}

	auto transition_ptr_cpu_offset{ world_palette_cpu_offset + c::WORLD_PALETTE_BYTE_SIZE };

	auto trans_x_cpu_addr{ p_manager.read_word(p_rom, p_bank_no, transition_ptr_cpu_offset) };
	auto trans_y_cpu_addr{ p_manager.read_word(p_rom, p_bank_no, transition_ptr_cpu_offset + 2) };
	auto trans_dest_cpu_addr{ p_manager.read_word(p_rom, p_bank_no, transition_ptr_cpu_offset + 4) };
	auto trans_ret_cpu_addr{ p_manager.read_word(p_rom, p_bank_no, transition_ptr_cpu_offset + 6) };
	std::size_t l_byte_cnt{ static_cast<std::size_t>(trans_y_cpu_addr) - trans_x_cpu_addr };

	world.door_x = p_manager.read_bytes(p_rom, p_bank_no, trans_x_cpu_addr, l_byte_cnt);
	world.door_y = p_manager.read_bytes(p_rom, p_bank_no, trans_y_cpu_addr, l_byte_cnt);
	world.door_dest = p_manager.read_bytes(p_rom, p_bank_no, trans_dest_cpu_addr, l_byte_cnt);
	world.door_ret = p_manager.read_bytes(p_rom, p_bank_no, trans_ret_cpu_addr, l_byte_cnt);

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

		// parse connections/transitions
		auto scr_trans_ptr_offset{ screen_ptrs_offset + 2 * 3 * i + 2 };
		auto screen_trans_cpu_offset{ p_manager.read_word(p_rom, p_bank_no, scr_trans_ptr_offset) };
		auto scr_trans_rom_offset{ p_manager.cpu_addr_to_rom_offset(p_bank_no, screen_trans_cpu_offset) };

		screen.minimap_x = p_rom.at(scr_trans_rom_offset);
		screen.minimap_y = p_rom.at(scr_trans_rom_offset + 1);
		screen.scroll_left = p_rom.at(scr_trans_rom_offset + 2) - 1;
		screen.scroll_right = p_rom.at(scr_trans_rom_offset + 3) - 1;

		for (std::size_t conn{ 4 }; ; ++conn) {
			byte b{ p_rom.at(scr_trans_rom_offset + conn) };

			if (b == 0x80)
				break;
			else if (b > 0x80) {
				screen.door_idxs.push_back((b & 0x7f) - 1);
				break;
			}
			else
				screen.door_idxs.push_back(b - 1);
		}

		// parse sprites
		auto scr_sprite_ptr_offset{ screen_ptrs_offset + 2 * 3 * i + 4 };
		auto screen_sprite_cpu_offset{ p_manager.read_word(p_rom, p_bank_no, scr_sprite_ptr_offset) };
		auto scr_sprite_rom_offset{ p_manager.cpu_addr_to_rom_offset(p_bank_no, screen_sprite_cpu_offset) };

		for (std::size_t spr{ 0 }; ; spr += 3) {
			bool last_entry{ false };

			byte b{ p_rom.at(scr_sprite_rom_offset + spr) };

			if (b == 0x80)
				break;
			else if (b > 0x80) {
				last_entry = true;
				b -= 0x80;
			}

			screen.sprites.push_back(Sprite{
				.id = b,
				.x_tile = p_rom.at(scr_sprite_rom_offset + spr + 1),
				.y_pixel = p_rom.at(scr_sprite_rom_offset + spr + 2)
				});

			if (last_entry)
				break;
		}

		// store screen
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

void boo::ROMParser::parse_sprite_palette_overrides(const std::vector<byte>& p_rom,
	const rom::ROM_Manager& p_manager,
	boo::Game& game) const {
	// TODO: Keep an eye on this - try to relocate when implementing patching
	// hard coded in asm to be contiguous in bank 1, from master ptr 6
	constexpr std::size_t SPRPAL_BANK{ 1 };
	constexpr std::size_t SPRPAL_FIRST_PTR_ADDR{ 0x8000 + 2 * 0x06 };

	for (std::size_t i{ 0 }; i < game.worlds.size(); ++i) {
		auto cpu_addr{ p_manager.read_word(p_rom, SPRPAL_BANK, SPRPAL_FIRST_PTR_ADDR + 2 * i) };
		auto paldata{ p_manager.read_bytes(p_rom, SPRPAL_BANK, cpu_addr,
			2 * game.worlds[i].screens.size()) };

		for (std::size_t j{ 0 }; j < game.worlds[i].screens.size(); ++j) {
			game.worlds[i].screens[j].sprite_pal0_offset = paldata[j * 2];
			game.worlds[i].screens[j].sprite_pal1_offset = paldata[j * 2 + 1];
		}
	}
}

boo::Tile boo::ROMParser::decode_packed_sprite_tile(byte p_val) const {
	return Tile{
		.idx = static_cast<byte>(p_val & 0x3f),
		.pal = static_cast<byte>((p_val >> 6) & 0x03),
		.v_flip = false,
		.h_flip = false
	};
}

void boo::ROMParser::append_frame_col_from_descriptor(boo::AnimationFrame& p_frame,
	const std::vector<byte>& p_rom,
	std::size_t tile_strm_rom_offset,
	byte descriptor_index) const {
	const auto descriptor_offset{ tile_strm_rom_offset + descriptor_index };

	const auto column_height{ p_rom.at(descriptor_offset) };
	const auto tile_data_offset{ tile_strm_rom_offset + p_rom.at(descriptor_offset + 1) };

	p_frame.append_column(p_rom, tile_data_offset, column_height);
}

boo::AnimationFrame boo::ROMParser::parse_inline_descriptor_frame(
	const std::vector<byte>& p_rom, std::size_t descriptor_block_offset,
	std::size_t payload_base_offset, std::size_t column_count) const {
	boo::AnimationFrame result;

	for (std::size_t i{ 0 }; i < column_count; ++i) {

		const auto column_height{ p_rom.at(descriptor_block_offset++) };
		const auto tile_data_offset{ payload_base_offset + p_rom.at(descriptor_block_offset++) };

		result.append_column(p_rom, tile_data_offset, static_cast<byte>(column_height));
	}

	return result;
}

boo::AnimationFrame boo::ROMParser::parse_split_column_descriptor_frame(
	const std::vector<byte>& rom, std::size_t frame_offset_table_rom_offset,
	std::size_t descriptor_stream_rom_offset, std::size_t packed_tile_stream_rom_offset,
	std::size_t column_y_rom_offset, std::size_t frame_index,
	std::size_t column_count) const {
	boo::AnimationFrame result;

	const auto descriptor_rel =
		rom.at(frame_offset_table_rom_offset + frame_index);

	auto descriptor_offset =
		descriptor_stream_rom_offset + descriptor_rel;

	for (std::size_t col{ 0 }; col < column_count; ++col) {

		const auto y_offset =
			rom.at(column_y_rom_offset + col);

		const auto height =
			rom.at(descriptor_offset++);

		const auto payload_offset =
			rom.at(descriptor_offset++);

		result.append_column(rom, packed_tile_stream_rom_offset + payload_offset,
			static_cast<byte>(height), y_offset / 8);
	}

	return result;
}

boo::AnimationFrame
boo::ROMParser::parse_split_column_descriptor_frame_fixed_stride(
	const std::vector<byte>& rom,
	std::size_t descriptor_stream_rom_offset,
	std::size_t packed_tile_stream_rom_offset,
	std::size_t column_y_rom_offset,
	std::size_t column_y_layout_selector_rom_offset,
	std::size_t frame_index,
	std::size_t column_count) const {

	boo::AnimationFrame result;

	auto descriptor_offset{
		descriptor_stream_rom_offset +
		frame_index * column_count * 2
	};

	const auto layout_rel{
		rom.at(
			column_y_layout_selector_rom_offset +
			(frame_index % 3))
	};

	const auto effective_column_y_rom_offset{
		column_y_rom_offset + layout_rel
	};

	for (std::size_t col{ 0 }; col < column_count; ++col) {

		auto y_offset{
			rom.at(effective_column_y_rom_offset + col)
		};

		if ((frame_index == 1 || frame_index == 3) && col == 2) {
			y_offset += 8;
		}

		const auto height{
			rom.at(descriptor_offset++)
		};

		const auto payload_offset{
			rom.at(descriptor_offset++)
		};

		result.append_column(
			rom,
			packed_tile_stream_rom_offset + payload_offset,
			static_cast<byte>(height),
			y_offset / 8
		);
	}

	if (frame_index == 3)
		result.append_column({ 0x09 }, 0, 1, 3);

	return result;
}

boo::AnimationFrame boo::ROMParser::parse_column_layout_descriptor_frame(
	const std::vector<byte>& p_rom,
	std::size_t descriptor_block_offset,
	std::size_t payload_base_offset,
	std::size_t column_y_offset,
	std::size_t column_count) const {
	boo::AnimationFrame result;

	for (std::size_t col{ 0 }; col < column_count; ++col) {

		const auto y_offset_px{
			p_rom.at(column_y_offset + col)
		};

		const auto column_height{
			p_rom.at(descriptor_block_offset++)
		};

		const auto tile_data_offset{
			payload_base_offset +
			p_rom.at(descriptor_block_offset++)
		};

		result.append_column(
			p_rom,
			tile_data_offset,
			static_cast<byte>(column_height),
			static_cast<std::size_t>(y_offset_px / 8)
		);
	}

	return result;
}

boo::AnimationFrame boo::ROMParser::parse_cyclops_frame(
	const std::vector<byte>& rom,
	std::size_t frame_offset_table_rom_offset,
	std::size_t data_base_rom_offset,
	std::size_t frame_index) const {

	AnimationFrame result;

	const auto descriptor_rel{
		rom.at(frame_offset_table_rom_offset + frame_index)
	};

	auto descriptor_offset{
		data_base_rom_offset + descriptor_rel
	};

	for (std::size_t col{ 0 }; col < 3; ++col) {

		const auto height{
			rom.at(descriptor_offset++)
		};

		const auto payload_rel{
			rom.at(descriptor_offset++)
		};

		result.append_column(
			rom,
			data_base_rom_offset + payload_rel,
			static_cast<byte>(height)
		);
	}

	return result;
}

boo::AnimationFrame boo::ROMParser::parse_bull_frame(const std::vector<byte>& p_rom,
	std::size_t frame_data_rom_offset, std::size_t tile_strm_rom_offset,
	bool append_special) const {
	boo::AnimationFrame result;

	// first entry expands into 3 consecutive descriptors.
	const auto first_descriptor{ p_rom.at(frame_data_rom_offset++) };
	for (std::size_t i{ 0 }; i < 3; ++i)
		append_frame_col_from_descriptor(result, p_rom,
			tile_strm_rom_offset, first_descriptor + 2 * i);

	// remaining two are standalone descriptors.
	append_frame_col_from_descriptor(result, p_rom,
		tile_strm_rom_offset,
		p_rom.at(frame_data_rom_offset++));
	append_frame_col_from_descriptor(result, p_rom,
		tile_strm_rom_offset,
		p_rom.at(frame_data_rom_offset++));

	if (append_special) {
		result.append_column(p_rom,
			tile_strm_rom_offset + 0x29,
			2, 2);
	}

	return result;
}

std::vector<boo::AnimationFrame> boo::ROMParser::parse_two_tile_frames(byte start_index,
	std::size_t p_count, byte p_sub_palette) const {
	std::vector<boo::AnimationFrame> result;

	for (std::size_t i{ 0 }; i < p_count; ++i) {
		boo::AnimationFrame frame;
		frame.initialize(2, 1);
		frame.tilemap[0][0] = Tile{
			.idx = static_cast<byte>(start_index + 2 * i),
			.pal = p_sub_palette,
			.v_flip = false,
			.h_flip = false };
		frame.tilemap[0][1] = Tile{
			.idx = static_cast<byte>(start_index + 2 * i + 1),
			.pal = p_sub_palette,
			.v_flip = false,
			.h_flip = false };

		result.push_back(frame);
	}

	return result;
}

std::vector<boo::AnimationFrame> boo::ROMParser::parse_mirrored_tile_frames(byte start_index,
	std::size_t p_count, byte p_sub_palette) const {
	std::vector<boo::AnimationFrame> result;

	for (std::size_t i{ 0 }; i < p_count; ++i) {
		boo::AnimationFrame frame;
		frame.initialize(2, 1);
		frame.tilemap[0][0] = Tile{
			.idx = static_cast<byte>(start_index + i),
			.pal = p_sub_palette,
			.v_flip = false,
			.h_flip = false };
		frame.tilemap[0][1] = Tile{
			.idx = static_cast<byte>(start_index + i),
			.pal = p_sub_palette,
			.v_flip = false,
			.h_flip = true };

		result.push_back(frame);
	}

	return result;
}

std::vector<boo::AnimationFrame> boo::ROMParser::parse_single_flip_tile_frames(byte start_index,
	byte p_sub_palette) const {
	std::vector<boo::AnimationFrame> result;

	for (std::size_t i{ 0 }; i < 2; ++i) {
		boo::AnimationFrame frame;
		frame.initialize(1, 1);
		frame.tilemap[0][0] = Tile{
			.idx = static_cast<byte>(start_index),
			.pal = p_sub_palette,
			.v_flip = false,
			.h_flip = (i == 1) };
		result.push_back(frame);
	}

	return result;
}

std::vector<boo::AnimationFrame> boo::ROMParser::parse_animation_frames(const std::vector<byte>& p_rom,
	const rom::ROM_Manager& p_manager, byte p_sprite_id, const ht::AnimationConfig& p_config) const {
	std::vector<boo::AnimationFrame> frames;

	std::optional<std::size_t> frame_rom_offset, tile_strm_rom_offset;
	byte BANK_NO{ p_config.bank_override ? static_cast<byte>(0x07) : c::SPRITE_FRAME_BANK_NO };

	if (p_config.frame_def_addr)
		frame_rom_offset = p_manager.cpu_addr_to_rom_offset(BANK_NO,
			p_config.frame_def_addr.value());
	if (p_config.tile_entry_addr)
		tile_strm_rom_offset = p_manager.cpu_addr_to_rom_offset(BANK_NO,
			p_config.tile_entry_addr.value());

	if (p_config.style == ht::AnimationStyle::BullStyle) {
		for (std::size_t i{ 0 }; i < p_config.frame_count; ++i)
			frames.push_back(parse_bull_frame(p_rom, frame_rom_offset.value() + 3 * i,
				tile_strm_rom_offset.value(), i == 5 || i == 6));
	}
	else if (p_config.style == ht::AnimationStyle::StriderStyle) {

		for (std::size_t i{ 0 }; i < p_config.frame_count; ++i) {
			const auto descriptor_block_offset{
				tile_strm_rom_offset.value() +
				p_rom.at(frame_rom_offset.value() + i)
			};

			frames.push_back(parse_inline_descriptor_frame(
				p_rom,
				descriptor_block_offset,
				tile_strm_rom_offset.value(),
				2
			));
		}
	}
	else if (p_config.style == ht::AnimationStyle::SequentialDescriptorFrames) {

		for (std::size_t i{ 0 }; i < p_config.frame_count; ++i) {

			const auto descriptor_block_offset{
				frame_rom_offset.value() + i * 2 * p_config.column_count.value_or(4)
			};

			frames.push_back(
				parse_inline_descriptor_frame(
					p_rom,
					descriptor_block_offset,
					tile_strm_rom_offset.value(),
					p_config.column_count.value()
				)
			);
		}
	}
	else if (p_config.style == ht::AnimationStyle::SplitColumnDescriptor) {

		const auto column_y_rom_offset{
			p_manager.cpu_addr_to_rom_offset(
				c::SPRITE_FRAME_BANK_NO,
				p_config.column_y_addr.value())
		};

		for (std::size_t i{ 0 }; i < p_config.frame_count; ++i) {

			frames.push_back(
				parse_split_column_descriptor_frame(
					p_rom,
					frame_rom_offset.value(),          // frame descriptor offset table
					tile_strm_rom_offset.value(),      // descriptor stream base
					tile_strm_rom_offset.value(),      // packed tile stream base
					column_y_rom_offset,
					i,
					p_config.column_count.value()
				)
			);
		}
	}
	else if (p_config.style == ht::AnimationStyle::ColumnLayoutDescriptorFrames) {

		const auto column_y_rom_offset{
			p_manager.cpu_addr_to_rom_offset(
				c::SPRITE_FRAME_BANK_NO,
				p_config.column_y_addr.value())
		};

		const auto column_count_rom_offset{
			p_manager.cpu_addr_to_rom_offset(
				c::SPRITE_FRAME_BANK_NO,
				p_config.column_count_addr.value())
		};

		for (std::size_t i{ 0 }; i < p_config.frame_count; ++i) {

			const auto column_count{
				static_cast<std::size_t>(
					p_rom.at(column_count_rom_offset + i))
			};

			const auto descriptor_offset{
				tile_strm_rom_offset.value() +
				p_rom.at(frame_rom_offset.value() + i)
			};

			frames.push_back(
				parse_column_layout_descriptor_frame(
					p_rom,
					descriptor_offset,
					tile_strm_rom_offset.value(),
					column_y_rom_offset,
					column_count
				)
			);
		}
	}
	else if (p_config.style == ht::AnimationStyle::TwoColumnFixedFrames) {

		std::vector<byte> frame_offsets;

		if (p_config.frame_def_addr) {
			frame_offsets =
				p_manager.read_bytes(
					p_rom,
					BANK_NO,
					p_config.frame_def_addr.value(),
					p_config.frame_count
				);
		}

		for (std::size_t i{ 0 }; i < p_config.frame_count; ++i) {

			AnimationFrame frame;
			frame.initialize(2, 2);

			const auto tile_offset{
				tile_strm_rom_offset.value() +
				(
					p_config.frame_def_addr
					? frame_offsets[i]
					: static_cast<byte>(i * 4)
				)
			};

			for (std::size_t y{ 0 }; y < 2; ++y) {
				for (std::size_t x{ 0 }; x < 2; ++x) {

					frame.tilemap[y][x] =
						decode_packed_sprite_tile(
							p_rom.at(tile_offset + y * 2 + x)
						);
				}
			}

			frames.push_back(std::move(frame));
		}

	}
	else if (p_config.style == ht::AnimationStyle::MirroredQuadrant2x2) {
		AnimationFrame frame;
		frame.initialize(4, 4);

		auto decode = [&](std::size_t offs) {
			return decode_packed_sprite_tile(
				p_rom.at(tile_strm_rom_offset.value() + offs)
			);
			};

		// frame 0
		std::array<Tile, 4> f0{
			decode(0),
			decode(1),
			decode(2),
			decode(3)
		};

		// frame 1
		std::array<Tile, 4> f1{
			decode(4),
			decode(5),
			decode(6),
			decode(7)
		};

		auto apply = [](Tile t, bool h, bool v) {

			t.h_flip ^= h;
			t.v_flip ^= v;
			return t;
			};

		for (std::size_t i{ 0 }; i < 4; ++i) {

			const auto x{ i % 2 };
			const auto y{ i / 2 };

			// top-left
			frame.tilemap[1 - y][x] =
				apply(f0[i], false, true);

			// top-right
			frame.tilemap[1 - y][x + 2] =
				apply(f1[i], true, true);

			// bottom-left
			frame.tilemap[y + 2][x] =
				f0[i];

			// bottom-right
			frame.tilemap[y + 2][x + 2] =
				apply(f1[i], true, false);
		}

		frames.push_back(std::move(frame));
	}
	else if (p_config.style == ht::AnimationStyle::CyclopsStyle) {

		for (std::size_t i{ 0 }; i < p_config.frame_count; ++i) {

			frames.push_back(
				parse_cyclops_frame(
					p_rom,
					frame_rom_offset.value(),
					tile_strm_rom_offset.value(),
					i
				)
			);
		}
	}
	else if (p_config.style == ht::AnimationStyle::BubbleStyle) {

		for (std::size_t i{ 0 }; i < 2; ++i) {

			AnimationFrame frame;
			frame.initialize(2, 2);

			// lower animated pair
			frame.tilemap[1][0] = Tile{
				.idx = static_cast<byte>(i),
				.pal = 1
			};

			frame.tilemap[1][1] = Tile{
				.idx = static_cast<byte>(i),
				.pal = 1
			};

			// top bubble cap
			frame.tilemap[0][0] = Tile{
				.idx = 2,
				.pal = 1
			};

			// empty top-right
			frame.tilemap[0][1] = Tile{
				.idx = 0xff
			};

			frames.push_back(frame);
		}
	}
	else if (p_config.style == ht::AnimationStyle::TwoTileContiguous) {
		return parse_two_tile_frames(p_config.start_chr_index.value_or(0),
			p_config.frame_count,
			p_config.sub_palette.value_or(0));
	}
	else if (p_config.style == ht::AnimationStyle::MirroredTile) {
		return parse_mirrored_tile_frames(0, p_config.frame_count, 0x01);
	}
	else if (p_config.style == ht::AnimationStyle::SingleTileFlipAnimated) {
		return parse_single_flip_tile_frames(0, 0x03);
	}
	else if (p_config.style == ht::AnimationStyle::SingleStaticTile) {

		boo::AnimationFrame frame;
		frame.initialize(1, 1);

		frame.tilemap[0][0] = Tile{
			.idx = static_cast<byte>(p_config.start_chr_index.value_or(0)),
			.pal = static_cast<byte>(p_config.sub_palette.value_or(0))
		};

		frames.push_back(frame);
	}
	else if (p_config.style == ht::AnimationStyle::ItemType) {

		boo::AnimationFrame frame;
		frame.initialize(2, 2);

		byte subpal{ static_cast<byte>(p_config.sub_palette.value_or(0)) };
		byte chr_offset{ static_cast<byte>(p_config.start_chr_index.value_or(0)) };

		for (byte b{ 0 }; b < 4; ++b) {
			frame.tilemap[b / 2][b % 2] = Tile{
				.idx = static_cast<byte>(b + chr_offset),
				.pal = subpal
			};
		}

		frames.push_back(frame);
	}
	else if (p_config.style ==
		ht::AnimationStyle::LionStyle) {

		const auto column_y_rom_offset{
			p_manager.cpu_addr_to_rom_offset(
				BANK_NO,
				p_config.column_y_addr.value())
		};

		const auto column_y_layout_selector_rom_offset{
			p_manager.cpu_addr_to_rom_offset(
				BANK_NO,
				p_config.frame_def_addr.value())
		};

		for (std::size_t i{ 0 }; i < p_config.frame_count; ++i) {

			frames.push_back(
				parse_split_column_descriptor_frame_fixed_stride(
					p_rom,
					frame_rom_offset.value(),
					tile_strm_rom_offset.value(),
					column_y_rom_offset,
					column_y_layout_selector_rom_offset,
					i,
					p_config.column_count.value()
				)
			);
		}
	}
	else
		throw std::runtime_error(std::format("Animation frame parser for sprite with id ${:02x} not implemented", p_sprite_id));

	return frames;
}
