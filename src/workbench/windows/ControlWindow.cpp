#include "MainWindow.h"
#include <core/rom/ROM_Manager.h>
#include <core/klib/Kfile.h>
#include "./../ext/imgui/imgui.h"
#include "./../ext/imgui/imgui_impl_sdl3.h"
#include "./../ext/imgui/imgui_impl_sdlrenderer3.h"
#include "imgui_helper.h"

void htw::MainWindow::draw_control_window(SDL_Renderer* p_rnd) {
	ImGui::Begin("Project Control");

	if (ui::imgui_button("Expand ROM", 4)) try {
		const auto expaneded_rom{ rom::ROM_Manager::expand_rom(rom) };
		klib::file::write_bytes_to_file(expaneded_rom, "c:/temp/boo/boo-us-exp.nes");
		add_message("Expanded ROM file written", 2);
	}
	catch (const std::exception& ex) {
		add_message(ex.what(), 1);
	}

	show_messages();

	ImGui::End();
}

void htw::MainWindow::show_messages(void) const {
	ImGui::SeparatorText("Output Messages");
	for (const auto& msg : messages) {
		ImGui::PushStyleColor(ImGuiCol_Text, ui::g_uiStyles[msg.color_idx].active);
		ImGui::TextUnformatted(msg.text.c_str());
		ImGui::PopStyleColor();
	}
}
