#include "Palette.h"

std::vector<byte> nes::Palette::get_flat_palette(void) const {
	std::vector<byte> result;
	for (const auto& subpal : colors)
		result.insert(end(result), begin(subpal), end(subpal));
	return result;
}
