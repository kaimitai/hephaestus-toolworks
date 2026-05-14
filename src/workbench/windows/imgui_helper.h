#ifndef HTW_IMGUI_HELPER_H
#define HTW_IMGUI_HELPER_H

#include <cstdint>
#include <string>
#include <vector>
#include "./../ext/imgui/imgui.h"
#include "./../ext/imgui/imgui_impl_sdl3.h"
#include "./../ext/imgui/imgui_impl_sdlrenderer3.h"

using byte = uint8_t;

namespace htw {

	namespace ui {

		struct UIStyle {
			ImVec4 normal;
			ImVec4 hovered;
			ImVec4 active;
		};

		extern std::vector<UIStyle> g_uiStyles;

		bool imgui_button(const std::string& p_label, byte p_style = 0,
			const std::string& p_tooltip = std::string(), bool p_disabled = false);
	}

}

#endif
