#ifndef BOO_ROMPARSER_H
#define BOO_ROMPARSER_H

#include <cstdint>
#include <vector>
#include "./../rom/ROM_Manager.h"
#include "World.h"
#include "Metatile.h"
#include "boo_constants.h"

using byte = uint8_t;

namespace boo {

	class ROMParser {

		World parse_world(const std::vector<byte>& p_rom,
			const rom::ROM_Manager& p_manager,
			std::size_t p_bank_no, std::size_t p_ptr_no) const;
		std::array<Metatile, c::WORLD_METATILE_COUNT> parse_metatiles(const std::vector<byte>& p_rom,
			std::size_t p_offset) const;

	public:
		ROMParser(void) = default;
		World parse(const std::vector<byte>& p_rom,
			std::size_t p_bank_no, std::size_t p_ptr_no) const;
	};

}

#endif
