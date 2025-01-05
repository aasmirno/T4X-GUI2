#pragma once

// imgui includes
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>

struct Menu {
	/*
		Create imgui menu
	*/
	virtual void draw() = 0;
};

struct MainMenu : public Menu {
	virtual void draw() {
		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoTitleBar;
		window_flags |= ImGuiWindowFlags_NoScrollbar;
		//window_flags |= ImGuiWindowFlags_MenuBar;
		window_flags |= ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoResize;
		window_flags |= ImGuiWindowFlags_NoCollapse;
		window_flags |= ImGuiWindowFlags_NoBackground;

		ImVec2 button_size = ImVec2(100, 50);

		ImGui::SetNextWindowSize(ImVec2(500, 500));
		ImGui::Begin("Main Menu", NULL, window_flags);
		ImGui::Button("New", button_size);
		ImGui::Button("Load", button_size);
		ImGui::Button("Exit", button_size);
		ImGui::End();
	}
};