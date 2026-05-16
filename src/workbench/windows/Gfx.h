#ifndef HTW_GFX_H
#define HTW_GFX_H

#include <SDL3/SDL.h>
#include <unordered_map>
#include <vector>
#include <core/nes/ChrTile.h>
#include <core/nes/Palette.h>
#include <core/boo/Metatile.h>
#include <core/boo/Screen.h>
#include <core/boo/AnimationFrame.h>

namespace htw {

	struct Gfx {

		SDL_Palette* nes_palette;

		// background tile atlas, tiles will be pulled from here when rendering the background gfx of a screen
		// based on a vector<NES tile> and a NES palette with 4 sub-palettes
		// 256 x 4 tiles, one row of tiles per sub-palette. Total dimensions (256*8 x 4x8) = 1024x32 pixels
		SDL_Texture* atlas;
		SDL_Texture* screen;
		std::unordered_map<byte, std::vector<SDL_Texture*>> sprite_frames;
		std::vector<SDL_Texture*> metatiles;

		SDL_Color m_hot_pink{ SDL_Color(0xff, 0x69, 0xb4, 0x00) };

		// SDL common
		SDL_Surface* create_sdl_surface(int p_w, int p_h,
			bool p_transparent = false, bool p_set_no_colorkey = false) const;
		SDL_Texture* surface_to_texture(SDL_Renderer* p_rnd, SDL_Surface* p_srf,
			bool p_destroy_surface = true) const;
		void delete_texture(SDL_Texture* txt);

		// nes rendering
		void put_nes_pixel(SDL_Surface* srf, int x, int y, byte p_palette_index,
			bool p_transparent = false) const;
		void draw_nes_tile_on_surface(SDL_Surface* p_srf, int dst_x, int dst_y,
			const nes::ChrTile& tile, const std::vector<byte>& p_palette,
			bool p_transparent, bool h_flip, bool v_flip) const;

		// sprite rendering
		SDL_Surface* gen_sprite_frame_surface(const boo::AnimationFrame& p_frame,
			const std::vector<nes::ChrTile> p_tiles,
			const std::vector<std::vector<byte>>& p_palette) const;
		void draw_sprite_on_screen(SDL_Renderer* p_rnd, byte p_sprite_no, int x, int y);

	public:
		Gfx(SDL_Renderer* p_rnd);
		~Gfx(void);

		void render_tilemap_to_screen(SDL_Renderer* p_rnd, const boo::Screen& p_screen,
			std::size_t p_offset);

		void draw_rect_on_screen(SDL_Renderer* p_rnd, SDL_Color p_color, int x, int y, int w, int h) const;
		void draw_sprite_on_screen(SDL_Renderer* p_rnd, byte p_sprite_no,
			std::size_t p_frame_no, int x, int y);

		void initialize_palette(const std::vector<SDL_Color>& p_colors);
		void generate_metatile_textures(SDL_Renderer* p_rnd, const std::vector<boo::Metatile>& p_metatiles);
		void generate_metatile_texture(SDL_Renderer* p_rnd, std::size_t p_idx, const boo::Metatile& p_metatile);
		void generate_atlas(SDL_Renderer* p_rnd,
			const std::vector<nes::ChrTile>& tiles,
			const nes::Palette& palette);

		void generate_sprite_frame_textures(SDL_Renderer* p_rnd,
			byte p_sprite_id, const std::vector<nes::ChrTile>& tiles, const nes::Palette& palette,
			const std::vector<boo::AnimationFrame>& frames);
	};

}

#endif
