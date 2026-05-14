#ifndef HTW_GFX_H
#define HTW_GFX_H

#include <SDL3/SDL.h>
#include <vector>
#include <core/nes/ChrTile.h>
#include <core/nes/Palette.h>
#include <core/boo/Metatile.h>
#include <core/boo/Screen.h>

namespace htw {

	struct Gfx {

		SDL_Palette* nes_palette;

		// background tile atlas, tiles will be pulled from here when rendering the background gfx of a screen
		// based on a vector<NES tile> and a NES palette with 4 sub-palettes
		// 256 x 4 tiles, one row of tiles per sub-palette. Total dimensions (256*8 x 4x8) = 1024x32 pixels
		SDL_Texture* atlas;
		SDL_Texture* screen;
		std::vector<SDL_Texture*> metatiles;

		SDL_Color m_hot_pink{ 0x10, 0x20, 0x30, 0xff };

		void delete_texture(SDL_Texture* txt);

	public:
		Gfx(SDL_Renderer* p_rnd);
		~Gfx(void);

		void render_tilemap_to_screen(SDL_Renderer* p_rnd, const boo::Screen& p_screen,
			std::size_t p_offset);

		void draw_rect_on_screen(SDL_Renderer* p_rnd, SDL_Color p_color, int x, int y, int w, int h) const;

		void initialize_palette(const std::vector<SDL_Color>& p_colors);
		void generate_metatile_textures(SDL_Renderer* p_rnd, const std::vector<boo::Metatile>& p_metatiles);
		void generate_metatile_texture(SDL_Renderer* p_rnd, std::size_t p_idx, const boo::Metatile& p_metatile);
		void generate_atlas(SDL_Renderer* p_rnd,
			const std::vector<nes::ChrTile>& tiles,
			const nes::Palette& palette);
		void put_nes_pixel(SDL_Surface* srf, int x, int y, byte p_palette_index,
			bool p_transparent = false) const;

		SDL_Surface* create_sdl_surface(int p_w, int p_h,
			bool p_transparent = false, bool p_set_no_colorkey = false) const;
		SDL_Texture* surface_to_texture(SDL_Renderer* p_rnd, SDL_Surface* p_srf,
			bool p_destroy_surface = true) const;
	};

}

#endif
