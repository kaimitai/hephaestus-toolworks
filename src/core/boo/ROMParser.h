#ifndef BOO_ROMPARSER_H
#define BOO_ROMPARSER_H

#include <cstdint>
#include <vector>
#include "./../rom/ROM_Manager.h"
#include "Game.h"
#include "World.h"
#include "Metatile.h"
#include "boo_constants.h"
#include "./../htConfig.h"

using byte = uint8_t;

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

	public:
		ROMParser(void) = default;
		Game parse(const ht::htConfig& p_config, const std::vector<byte>& p_rom) const;
	};

}

#endif
