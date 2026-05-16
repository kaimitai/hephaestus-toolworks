#include "MainWindow.h"
#include <core/boo/ROMParser.h>
#include <core/klib/Kfile.h>
#include <algorithm>
#include <format>
#include <unordered_set>
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

	rom = klib::file::read_file_as_bytes("c:/temp/boo/boo-us.nes");
	boo::ROMParser parser;
	game = parser.parse(config, rom);

}

void htw::MainWindow::draw(SDL_Renderer* p_rnd) {
	clock.update();

	if (gfx_state.redraw) {
		gfx.generate_atlas(p_rnd,
			game->get_world_tileset(sel.world_no),
			game->get_screen_bg_palette(sel.world_no, sel.screen_no,
				!settings.render_with_world_palette));
		gfx.generate_metatile_textures(p_rnd, game->worlds.at(sel.world_no).metatiles);

		std::unordered_set<byte> screen_sprites;
		for (const auto& sprite : game->worlds.at(sel.world_no).screens.at(sel.screen_no).sprites)
			screen_sprites.insert(sprite.id);

		for (byte sprite_id : screen_sprites) {
			auto iter{ game->sprite_animations.find(sprite_id) };
			if (iter != end(game->sprite_animations)) {
				gfx.generate_sprite_frame_textures(p_rnd, sprite_id,
					game->sprite_chr.at(config.get_sprite_chr_bank_id(sprite_id)),
					game->get_screen_sprite_palette(sel.world_no, sel.screen_no),
					game->sprite_animations.at(sprite_id));
			}
		}
		gfx_state.redraw = false;
	}

	SDL_SetRenderDrawColor(p_rnd, 0, 33, 71, 0);
	SDL_RenderClear(p_rnd);

	ImGui_ImplSDLRenderer3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	draw_tilemap_window(p_rnd);
	draw_control_window(p_rnd);

	ImGui::Render();
	ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), p_rnd);
}

void htw::MainWindow::add_message(const std::string& p_msg, byte p_color_idx, bool p_repeat) {
	if (messages.size() > 50)
		messages.pop_back();

	if (p_repeat || messages.empty() || messages.front().text != p_msg)
		messages.push_front(htw::Message(p_msg, p_color_idx));
}
