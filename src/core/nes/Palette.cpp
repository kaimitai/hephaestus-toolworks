#include "Palette.h"

byte nes::Palette::get_nes_color(std::size_t p_sub_palette, std::size_t p_color_no) const {
	return colors[p_sub_palette][p_color_no];
}

std::vector<byte> nes::Palette::get_flat_palette(void) const {
	std::vector<byte> result;
	for (const auto& subpal : colors)
		result.insert(end(result), begin(subpal), end(subpal));
	return result;
}
