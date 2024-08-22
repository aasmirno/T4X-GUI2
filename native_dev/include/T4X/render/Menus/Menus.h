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
		ImGui::Begin("Test");

		ImGui::End();
	}
};