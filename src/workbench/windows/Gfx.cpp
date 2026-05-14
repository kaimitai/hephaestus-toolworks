#include "Gfx.h"

htw::Gfx::Gfx(SDL_Renderer* p_rnd) :
	nes_palette{ SDL_CreatePalette(256) },
	metatiles{ std::vector<SDL_Texture*>(64, nullptr) },
	screen{ SDL_CreateTexture(p_rnd, SDL_PIXELFORMAT_ABGR8888,
		SDL_TEXTUREACCESS_TARGET, 16 * 16, 11 * 16) }
{

}

void htw::Gfx::render_tilemap_to_screen(SDL_Renderer* p_rnd, const boo::Screen& p_screen,
	std::size_t p_offset) {

	const auto& tilemap{ p_screen.tilemap };
	SDL_SetRenderTarget(p_rnd, screen);

	for (std::size_t j{ 0 }; j < tilemap.size(); ++j)
		for (std::size_t x{ 0 }; x < 16; ++x) {
			std::size_t i{ x + p_offset };
			if (i >= tilemap[j].size())
				break;

			SDL_FRect dst_rect = {
				static_cast<float>(x * 16),
				static_cast<float>(j * 16),
				16.0f,
				16.0f
			};

			SDL_RenderTexture(p_rnd, metatiles.at(tilemap[j][i] & 0x3f), nullptr, &dst_rect);
		}

	SDL_SetRenderTarget(p_rnd, nullptr);
}

void htw::Gfx::draw_rect_on_screen(SDL_Renderer* p_rnd, SDL_Color p_color, int pixel_x, int pixel_y, int pixel_w, int pixel_h) const {
	SDL_SetRenderTarget(p_rnd, screen);
	SDL_SetRenderDrawColor(p_rnd, p_color.r, p_color.g, p_color.b, p_color.a);

	SDL_FRect l_rect(static_cast<float>(pixel_x),
		static_cast<float>(pixel_y),
		static_cast<float>(pixel_w),
		static_cast<float>(pixel_h));

	SDL_RenderRect(p_rnd, &l_rect);
	SDL_SetRenderTarget(p_rnd, nullptr);
}

SDL_Surface* htw::Gfx::create_sdl_surface(int p_w, int p_h,
	bool p_transparent, bool p_set_no_colorkey) const {
	SDL_Surface* l_bmp = SDL_CreateSurface(p_w, p_h, SDL_PIXELFORMAT_ABGR8888);

	Uint32 l_hotpink_32 = SDL_MapRGBA(SDL_GetPixelFormatDetails(l_bmp->format),
		nullptr,
		1, 1, 1, 0xff);

	if (p_transparent) {
		SDL_FillSurfaceRect(l_bmp, nullptr, l_hotpink_32);
		if (!p_set_no_colorkey)
			SDL_SetSurfaceColorKey(l_bmp, true, l_hotpink_32);
	}

	return l_bmp;
}

void htw::Gfx::put_nes_pixel(SDL_Surface* srf, int x, int y, byte p_palette_index,
	bool p_transparent) const {
	SDL_Color l_col{ nes_palette->colors[p_palette_index] };

	if (!p_transparent)
		SDL_WriteSurfacePixel(srf, x, y, l_col.r, l_col.g, l_col.b, l_col.a);
	else
		SDL_WriteSurfacePixel(srf, x, y,
			m_hot_pink.r,
			m_hot_pink.g,
			m_hot_pink.b,
			m_hot_pink.a
		);
}

void htw::Gfx::generate_metatile_textures(SDL_Renderer* p_rnd,
	const std::vector<boo::Metatile>& p_metatiles) {
	for (std::size_t i{ 0 }; i < p_metatiles.size(); ++i)
		generate_metatile_texture(p_rnd, i, p_metatiles[i]);
}

void htw::Gfx::generate_metatile_texture(SDL_Renderer* p_rnd,
	std::size_t p_idx, const boo::Metatile& p_metatile) {

	SDL_Texture* metatile = SDL_CreateTexture(
		p_rnd,
		SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_TARGET,
		16, 16);

	SDL_SetTextureBlendMode(metatile, SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget(p_rnd, metatile);
	SDL_SetRenderDrawColor(p_rnd, 0, 0, 0, 0xff); // opaque background
	SDL_RenderClear(p_rnd);

	int l_sub_palette_no{ p_metatile.palette_no };

	// Draw the 4 tiles from the atlas
	for (int col = 0; col < 2; ++col) {
		for (int row = 0; row < 2; ++row) {
			int tileIndex = p_metatile.tilemap[2 * col + row];
			SDL_FRect src = {
				static_cast<float>(tileIndex * 8),
				static_cast<float>(l_sub_palette_no * 8),
				8.0f, 8.0f
			};
			SDL_FRect dst = {
				static_cast<float>(row * 8),
				static_cast<float>(col * 8),
				8.0f, 8.0f
			};
			SDL_RenderTexture(p_rnd, atlas, &src, &dst);
		}
	}

	SDL_SetRenderTarget(p_rnd, nullptr);

	// Store the texture
	delete_texture(metatiles[p_idx]);
	metatiles[p_idx] = metatile;
}

void htw::Gfx::generate_atlas(SDL_Renderer* p_rnd,
	const std::vector<nes::ChrTile>& tiles,
	const nes::Palette& palette) {


	auto l_srf{ create_sdl_surface(8 * static_cast<int>(tiles.size()),
		8 * static_cast<int>(palette.colors.size())) }; // each palette is 4 bytes long, but each NES tile is 8 pixels high

	// draw all tiles onto the surface, once for each sub-palette
	for (int p{ 0 }; p < static_cast<int>(palette.colors.size()); ++p)
		for (int t{ 0 }; t < static_cast<int>(tiles.size()); ++t) {
			// draw pixels

			const auto& tile = tiles[t];

			for (int y = 0; y < 8; ++y) {
				for (int x = 0; x < 8; ++x) {
					byte palette_index = tile.get_color(x, y); // 0–3
					byte nes_color_index = palette.get_nes_color(p, palette_index); // NES color index from sub-palette

					int draw_x = t * 8 + x;
					int draw_y = p * 8 + y;

					put_nes_pixel(l_srf, draw_x, draw_y, nes_color_index);
				}
			}
		}

	delete_texture(atlas);
	atlas = surface_to_texture(p_rnd, l_srf);
}

SDL_Texture* htw::Gfx::surface_to_texture(SDL_Renderer* p_rnd, SDL_Surface* p_srf, bool p_destroy_surface) const {
	SDL_Texture* result = SDL_CreateTextureFromSurface(p_rnd, p_srf);

	if (p_destroy_surface)
		SDL_DestroySurface(p_srf);

	return(result);
}

void htw::Gfx::initialize_palette(const std::vector<SDL_Color>& p_colors) {
	SDL_Color out_palette[256] = {};
	for (std::size_t i{ 0 }; i < p_colors.size(); ++i) {
		out_palette[i] = p_colors[i];
	}

	SDL_SetPaletteColors(nes_palette, out_palette, 0, 256);
}

void htw::Gfx::delete_texture(SDL_Texture* p_txt) {
	if (p_txt != nullptr)
		SDL_DestroyTexture(p_txt);
}

htw::Gfx::~Gfx(void) {
	if (nes_palette)
		SDL_DestroyPalette(nes_palette);
	delete_texture(atlas);
	delete_texture(screen);
	for (auto txt : metatiles)
		delete_texture(txt);
}
