#include "AnimationFrame.h"
#include <optional>

std::size_t boo::AnimationFrame::w(void) const {
	return h() == 0 ? 0 : tilemap[0].size();
}

std::size_t boo::AnimationFrame::h(void) const {
	return tilemap.size();
}

void boo::AnimationFrame::initialize(std::size_t w, std::size_t h) {
	tilemap = std::vector<std::vector<std::optional<Tile>>>(h,
		std::vector<std::optional<Tile>>(w, std::nullopt));
}

void boo::AnimationFrame::append_column(const std::vector<byte>& p_rom, std::size_t p_offset,
	byte p_column_height, std::size_t p_y_offset) {
	const auto col_h{ static_cast<std::size_t>(p_column_height) };
	const auto total_h{ p_y_offset + col_h };

	// ensure enough rows exist
	while (tilemap.size() < total_h)
		tilemap.emplace_back();

	// existing width before appending
	const auto old_w{ w() };

	// pad all existing rows to equal width before adding new column
	for (auto& row : tilemap) {
		while (row.size() < old_w)
			row.emplace_back(std::nullopt);
	}

	// append exactly one new column entry per row
	for (std::size_t y{ 0 }; y < tilemap.size(); ++y) {

		// before strip starts -> empty.
		if (y < p_y_offset) {
			tilemap[y].emplace_back(std::nullopt);
		}

		// inside strip -> tile
		else if (y < total_h) {
			const auto val{ p_rom.at(p_offset++) };

			if (val == 0xff) {
				tilemap[y].emplace_back(std::nullopt);
			}
			else {
				tilemap[y].emplace_back(Tile{
					.idx = static_cast<byte>(val & 0x3f),
					.pal = static_cast<byte>((val >> 6) & 0x03),
					.v_flip = false,
					.h_flip = false
					});
			}
		}

		// below strip -> empty
		else {
			tilemap[y].emplace_back(std::nullopt);
		}
	}
}
