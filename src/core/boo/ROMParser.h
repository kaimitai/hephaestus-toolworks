#ifndef BOO_ROMPARSER_H
#define BOO_ROMPARSER_H

#include <cstdint>
#include <vector>
#include "./../rom/ROM_Manager.h"
#include "Game.h"
#include "World.h"
#include "Metatile.h"
#include "AnimationFrame.h"
#include "boo_constants.h"
#include "./../htConfig.h"

using byte = uint8_t;
using cpu_addr = uint16_t;

namespace boo {

	class ROMParser {

		World parse_world(const std::vector<byte>& p_rom,
			const rom::ROM_Manager& p_manager,
			std::size_t p_bank_no, std::size_t p_ptr_no) const;
		std::vector<Metatile> parse_metatiles(const std::vector<byte>& p_rom,
			std::size_t p_offset, std::size_t p_metatile_count) const;
		std::vector<nes::ChrTile> parse_chr_tiles(const std::vector<byte>& p_rom,
			std::size_t p_rom_offset, std::size_t p_decompressed_byte_size,
			bool p_compressed = true, bool p_1bpp = false) const;
		void parse_sprite_palette_overrides(const std::vector<byte>& p_rom,
			const rom::ROM_Manager& p_manager,
			boo::Game& game) const;

		void append_frame_col_from_descriptor(boo::AnimationFrame& p_frame,
			const std::vector<byte>& p_rom,
			std::size_t tile_strm_rom_offset,
			byte descriptor_index) const;
		boo::AnimationFrame parse_inline_descriptor_frame(
			const std::vector<byte>& p_rom, std::size_t descriptor_block_offset,
			std::size_t payload_base_offset, std::size_t column_count) const;
		AnimationFrame parse_bull_frame(const std::vector<byte>& p_rom,
			std::size_t frame_data_rom_offset, std::size_t tile_strm_rom_offset,
			bool append_special) const;
		std::vector<AnimationFrame> parse_two_tile_frames(byte start_index,
			std::size_t p_count) const;

	public:
		ROMParser(void) = default;
		Game parse(const ht::htConfig& p_config, const std::vector<byte>& p_rom) const;
		std::vector<AnimationFrame> parse_animation_frames(const std::vector<byte>& p_rom,
			const rom::ROM_Manager& p_manager, byte p_sprite_id, const ht::AnimationConfig& p_config) const;
	};

}

#endif
