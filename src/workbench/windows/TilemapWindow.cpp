#include "MainWindow.h"
#include <core/klib/Kfile.h>
#include <algorithm>
#include <format>
#include <utility>
#include "./../ext/imgui/imgui.h"
#include "./../ext/imgui/imgui_impl_sdl3.h"
#include "./../ext/imgui/imgui_impl_sdlrenderer3.h"
#include "imgui_helper.h"

void htw::MainWindow::draw_tilemap_window(SDL_Renderer* p_rnd) {
	const auto& world{ game->worlds.at(sel.world_no) };
	const auto& screen{ world.screens.at(sel.screen_no) };

	gfx.render_tilemap_to_screen(p_rnd, screen, static_cast<std::size_t>(sel.x_offset));

	// draw doors
	for (byte b : screen.door_idxs) {
		byte xx{ world.door_x.at(b) };
		byte yy{ world.door_y.at(b) };

		gfx.draw_rect_on_screen(p_rnd, SDL_Color{ .r = 128, .g = 255, .b = 128, .a = 255 },
			(xx - sel.x_offset) * 16, (yy - 3) * 16, 16, 32);
	}

	// draw sprites
	std::vector<std::pair<int, int>> sprite_points;
	for (const auto& spr : screen.sprites) {
		int spr_x{ 8 * static_cast<int>(spr.x_tile) - static_cast<int>(16 * sel.x_offset) };
		int spr_y{ static_cast<int>(spr.y_pixel) - 48 };

		sprite_points.push_back(std::make_pair(
			spr_x,
			spr_y
		));

		gfx.draw_rect_on_screen(p_rnd, SDL_Color{ .r = 255, .g = 50, .b = 50, .a = 255 },
			spr_x, spr_y, 8, 8);

		auto iter{ game->sprite_animations.find(spr.id) };
		if (iter != end(game->sprite_animations)) {
			gfx.draw_sprite_on_screen(p_rnd, iter->first,
				clock.cycle_index(iter->second.size(), 0.2), spr_x, spr_y);
		}
	}

	ImGui::Begin("Tilemap");
	ImGui::Image(gfx.screen, ImVec2(16 * 64, 11 * 64));

	bool hovered = ImGui::IsItemHovered();
	bool clicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left);

	if (hovered) {
		float wheel = ImGui::GetIO().MouseWheel;

		if (wheel != 0.0f) {
			sel.x_offset -= static_cast<int>(2 * wheel); // invert if needed
			sel.x_offset = std::clamp(sel.x_offset, 0, static_cast<int>(screen.tilemap.at(0).size() - 16));
		}
		else if (clicked) {
			ImVec2 mouse = ImGui::GetIO().MousePos;
			ImVec2 top_left = ImGui::GetItemRectMin();

			float local_x = mouse.x - top_left.x;
			float local_y = mouse.y - top_left.y;
			float world_pixel_x = local_x + 16 * sel.x_offset;

			int mt_x = static_cast<int>(local_x) / 64;
			int mt_y = static_cast<int>(local_y) / 64;

			mt_x = std::clamp(mt_x, 0, 15);
			mt_y = std::clamp(mt_y, 0, 10);

			int world_x = mt_x + sel.x_offset;

			byte raw = screen.tilemap.at(mt_y).at(world_x);
			byte prop{ game->worlds.at(sel.world_no).metatiles.at(raw & 0x3f).properties };
			/*
			printf("(%d,%d) world_x=%d raw=%02X idx=%02X pal=%d hi=%d\n",
				mt_x, mt_y, world_x,
				raw,
				raw & 0x3f,
				raw & 0x3,
				raw >> 6
			);

			printf("(%d,%d) id=%02x, prop=%02X [%d %d %d %d %d %d]\n",
				world_x, mt_y,
				raw & 0x3f,
				prop,
				(prop >> 0) & 1,
				(prop >> 1) & 1,
				(prop >> 2) & 1,
				(prop >> 3) & 1,
				(prop >> 4) & 1,
				(prop >> 5) & 1
			);
			*/

			add_message(std::format("({},{}) [{}]", world_x, mt_y, 0x500 + world_x + mt_y * 48), 6);

			if (true || (prop & 0b10)) {
				for (byte b : screen.door_idxs) {
					byte xx{ world.door_x.at(b) };
					byte yy{ world.door_y.at(b) };

					if (world_x == xx && ((yy - 2 == mt_y) || (yy - 3 == mt_y))) {
						add_message(std::format("*** Door {} ***", b), 6);

						byte dest{ world.door_dest.at(b) };
						byte ret{ world.door_ret.at(b) };
						add_message(std::format("return=${:2x}, destination=${:2x}", dest, ret), 6);

						if (dest < world.door_dest.size()) {

						}
					}
				}

			}

			float world_px_x{ world_pixel_x / 4 };
			for (std::size_t i{ 0 }; i < sprite_points.size(); ++i)
				if (world_px_x >= sprite_points[i].first && world_px_x < sprite_points[i].first + 8 &&
					local_y / 4 >= sprite_points[i].second && local_y / 4 < sprite_points[i].second + 8) {
					add_message(std::format("ID=${:2x}", screen.sprites.at(i).id), 6);
				}

		}
	}

	int w{ static_cast<int>(sel.world_no) };
	int s{ static_cast<int>(sel.screen_no) };

	if (ImGui::SliderInt("###world", &w, 0, 11)) {
		sel.world_no = static_cast<std::size_t>(w);
		sel.screen_no = std::clamp(sel.screen_no, static_cast<std::size_t>(0),
			game->worlds.at(sel.world_no).screens.size() - 1);
		gfx_state.redraw = true;
	}

	if (ImGui::SliderInt("###screen", &s, 0, game->worlds.at(sel.world_no).screens.size() - 1)) {
		sel.screen_no = static_cast<std::size_t>(s);
		gfx_state.redraw = true;
	}

	if (ImGui::Button("Verify connections")) {
		for (std::size_t w{ 0 }; w < game->worlds.size(); ++w)
			for (std::size_t s{ 0 }; s < game->worlds[w].screens.size(); ++s) {
				const auto& sscr{ game->worlds[w].screens[s] };
				if (sscr.scroll_left != 0xff && sscr.scroll_left >= 16)
					add_message(std::format("scroll left: (w={},s=${:2x})", w, s), 6);
				if (sscr.scroll_right != 0xff && sscr.scroll_right >= 16)
					add_message(std::format("scroll right: (w={},s={:2x}))", w, s), 6);
			}
	}

	ImGui::Text(std::format("Minimap: {},{}", screen.minimap_x, screen.minimap_y).c_str());
	ImGui::Text(std::format("{} <-> {}", screen.scroll_left, screen.scroll_right).c_str());
	ImGui::Text(std::format("Sprites: {}", screen.sprites.size()).c_str());

	if (ui::imgui_checkbox("World Palette", settings.render_with_world_palette,
		"Render without screen-specific palette overrides"))
		gfx_state.redraw = true;

	ImGui::End();
}
