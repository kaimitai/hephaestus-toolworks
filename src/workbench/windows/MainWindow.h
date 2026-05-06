#ifndef HTW_MAINWINDOW_H
#define HTW_MAINWINDOW_H

#include <SDL3/SDL.h>
#include <core/boo/Game.h>
#include <core/htConfig.h>
#include <optional>
#include "Gfx.h"

namespace htw {

	class MainWindow {

		ht::htConfig config;
		std::optional<boo::Game> game;
		Gfx gfx;

	public:
		MainWindow(SDL_Renderer* p_rnd);
		void draw(SDL_Renderer* p_rnd);

	};

}

#endif
