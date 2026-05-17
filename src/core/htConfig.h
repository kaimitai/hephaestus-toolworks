#ifndef HTCONFIG_H
#define HTCONFIG_H

#include <cstdint>
#include <map>
#include <string>
#include <optional>
#include <unordered_map>
#include "./script/Opcode.h"

using byte = uint8_t;
using cpu_addr = uint16_t;

namespace ht {

	struct Address {
		byte bank;
		cpu_addr addr;
	};

	struct ScriptConfig {
		std::unordered_map<byte, script::Opcode> opcodes;
		std::map<script::ArgDomain, std::map<byte, std::string>> defines;
	};

	enum class AnimationStyle {
		BullStyle, StriderStyle, TwoTileContiguous, MirroredTile, CyclopsStyle,
		SequentialDescriptorFrames, SplitColumnDescriptor, ColumnLayoutDescriptorFrames,
		SingleTileFlipAnimated, TwoColumnFixedFrames, MirroredQuadrant2x2,
		BubbleStyle, SingleStaticTile, ItemType, LionStyle
	};

	struct AnimationConfig {
		byte frame_count;
		AnimationStyle style;

		std::optional<cpu_addr> frame_def_addr, tile_entry_addr,
			column_y_addr, column_count, column_count_addr,
			column_y_layout_selector_rom_offset,
			start_chr_index, sub_palette;
		bool bank_override{ false };
	};

	struct RGBColor {
		byte r, g, b;
	};

	class htConfig {

		ScriptConfig scriptConfig;
		std::unordered_map<std::string, Address> addresses;
		std::unordered_map<std::string, std::size_t> counts;
		std::unordered_map<byte, byte> sprite_chr_banks;
		std::unordered_map<byte, AnimationConfig> animation_config;
		std::vector<RGBColor> nes_palette;

	public:
		htConfig(void);
		const ScriptConfig& get_script_config(void) const;
		byte get_sprite_chr_bank_id(byte p_sprite_no) const;
		const std::unordered_map<byte, AnimationConfig>&
			get_animation_config(void) const;

		std::size_t count(const std::string& p_id) const;
		Address address(const std::string& p_id) const;
		const std::vector<RGBColor>& get_nes_palette(void) const;
	};

}

#endif
