#include "MainWindow.h"
#include <core/boo/ROMParser.h>
#include <core/klib/Kfile.h>
#include <algorithm>
#include <map>
#include "./../ext/imgui/imgui.h"
#include "./../ext/imgui/imgui_impl_sdl3.h"
#include "./../ext/imgui/imgui_impl_sdlrenderer3.h"

htw::MainWindow::MainWindow(SDL_Renderer* p_rnd) :
	gfx{ p_rnd }
{
	std::vector<SDL_Color> nes_palette;
	const auto& config_palette{ config.get_nes_palette() };

	for (const auto& col : config_palette)
		nes_palette.push_back(SDL_Color{
			.r = col.r,
			.g = col.g,
			.b = col.b,
			.a = 0xff
			});

	gfx.initialize_palette(nes_palette);

	boo::ROMParser parser;
	game = parser.parse(config, klib::file::read_file_as_bytes("c:/temp/boo/boo-us.nes"));

	std::map<byte, int> counts;

	for (std::size_t i{ 0 }; i < game->worlds.size(); ++i)
		for (std::size_t j{ 0 }; j < game->worlds[i].metatiles.size(); ++j) {
			byte props{ game->worlds[i].metatiles[j].properties };
			++counts[props];
		}

	for (std::size_t s{ 0 }; s < game->worlds[1].screens.size(); ++s) {
		const auto& tilemap{ game->worlds[1].screens[s].tilemap};
		for (std::size_t j{ 0 }; j < 11; ++j)
			for (std::size_t i{ 0 }; i < 48; ++i)
				if (tilemap[j][i] == 59)
					printf("screem %d (%d,%d)\n", s, i, j);
	}

}

void htw::MainWindow::draw(SDL_Renderer* p_rnd) {
	static int xoffset{ 0 };
	static bool redraw{ true };
	static std::size_t world_no{ 0 };
	static std::size_t screen_no{ 0 };

	if (redraw) {
		gfx.generate_atlas(p_rnd, game->get_world_tileset(world_no), game->get_screen_palette(world_no, screen_no));
		gfx.generate_metatile_textures(p_rnd, game->worlds.at(world_no).metatiles);
		redraw = false;
	}

	SDL_SetRenderDrawColor(p_rnd, 0, 33, 71, 0);
	SDL_RenderClear(p_rnd);

	ImGui_ImplSDLRenderer3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	const auto& screen{ game->worlds.at(world_no).screens.at(screen_no) };

	gfx.render_tilemap_to_screen(p_rnd, screen, static_cast<std::size_t>(xoffset));


	ImGui::Begin("test");
	ImGui::Image(gfx.screen, ImVec2(16 * 64, 11 * 64));

	bool hovered = ImGui::IsItemHovered();
	bool clicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left);

	if (hovered) {
		float wheel = ImGui::GetIO().MouseWheel;

		if (wheel != 0.0f) {
			xoffset -= static_cast<int>(2 * wheel); // invert if needed
			xoffset = std::clamp(xoffset, 0, static_cast<int>(screen.tilemap.at(0).size() - 16));
		}
		else if (clicked) {
			ImVec2 mouse = ImGui::GetIO().MousePos;
			ImVec2 top_left = ImGui::GetItemRectMin();

			float local_x = mouse.x - top_left.x;
			float local_y = mouse.y - top_left.y;

			int mt_x = static_cast<int>(local_x) / 64;
			int mt_y = static_cast<int>(local_y) / 64;

			mt_x = std::clamp(mt_x, 0, 15);
			mt_y = std::clamp(mt_y, 0, 10);

			int world_x = mt_x + xoffset;

			byte raw = screen.tilemap.at(mt_y).at(world_x);
			byte prop{ game->worlds.at(world_no).metatiles.at(raw & 0x3f).properties };
			/*
			printf("(%d,%d) world_x=%d raw=%02X idx=%02X pal=%d hi=%d\n",
				mt_x, mt_y, world_x,
				raw,
				raw & 0x3f,
				raw & 0x3,
				raw >> 6
			);
			*/
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
		}
	}

	int w{ static_cast<int>(world_no) };
	int s{ static_cast<int>(screen_no) };

	if (ImGui::SliderInt("###screen", &s, 0, game->worlds.at(world_no).screens.size() - 1)) {
		screen_no = static_cast<std::size_t>(s);
		redraw = true;
	}

	if (ImGui::SliderInt("###world", &w, 0, 11)) {
		world_no = static_cast<std::size_t>(w);
		screen_no = std::clamp(screen_no, static_cast<std::size_t>(0), game->worlds.at(world_no).screens.size() - 1);
		redraw = true;
	}

	ImGui::End();

	ImGui::Render();
	ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), p_rnd);
}
