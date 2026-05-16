#ifndef HTW_MAINWINDOW_H
#define HTW_MAINWINDOW_H

#include <SDL3/SDL.h>
#include <core/boo/Game.h>
#include <core/htConfig.h>
#include <cstdint>
#include <deque>
#include <optional>
#include <string>
#include "Gfx.h"
#include "Clock.h"
#include "./../EditorSettings.h"

using byte = uint8_t;

namespace htw {

	struct Message {
		std::string text;
		byte color_idx;
	};

	struct EditorGfxState {
		bool redraw{ true };
	};

	struct EditorSelectors {
		int x_offset{ 0 };
		std::size_t world_no{ 0 };
		std::size_t screen_no{ 0 };
	};

	class MainWindow {

		ht::htConfig config;
		std::vector<byte> rom;
		std::optional<boo::Game> game;
		Gfx gfx;

		std::deque<htw::Message> messages;
		EditorGfxState gfx_state;
		EditorSelectors sel;
		EditorSettings settings;
		Clock clock;

		void draw_tilemap_window(SDL_Renderer* p_rnd);
		void draw_control_window(SDL_Renderer* p_rnd);

		void add_message(const std::string& p_msg, byte p_color_idx, bool p_repeat = false);
		void show_messages(void) const;

	public:
		MainWindow(SDL_Renderer* p_rnd);
		void draw(SDL_Renderer* p_rnd);

	};

}

#endif
