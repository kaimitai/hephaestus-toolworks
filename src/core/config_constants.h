#ifndef HT_CONFIG_CONSTANTS_H
#define HT_CONFIG_CONSTANTS_H

#include <cstddef>

namespace ht {

	namespace c {

		constexpr char ID_WORLD_COUNT[]{ "world_count" };
		constexpr char ID_SPRITE_CHR_BANK_COUNT[]{ "sprite_chr_bank_count" };
		constexpr char ID_CHR_TRANSFERSCRIPT_COUNT[]{ "chr_transferscript_count" };

		constexpr char ID_SHARED_BG_PALETTE_ADDR[]{ "shared_bg_palette_offset" };
		constexpr char ID_CHR_TRANSFERSCRIPT_ADDR[]{ "chr_transferscript_addr" };
		constexpr char ID_WORLD_DEFINITIONS_ADDR[]{ "world_definitions_addr" };

	}

}

#endif
