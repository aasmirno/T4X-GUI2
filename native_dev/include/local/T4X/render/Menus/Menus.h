#pragma once
#include "T4X/input/Event.h"

// imgui includes
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>

// cstd includes
#include <functional>

struct Menu {
	std::function<void(Event)> event_handler;
	/*
		Create imgui menu
	*/
	virtual void draw() = 0;
	Menu(std::function<void(Event)> event_handle) {
		event_handler = event_handle;
	}
};

struct NewGameMenu : public Menu {
	NewGameMenu(std::function<void(Event)> event_handle) : Menu(event_handle) {

	}

	Event engine_event;
	virtual void draw() {
		ImGuiWindowFlags window_flags = 0;
		//window_flags |= ImGuiWindowFlags_NoTitleBar;
		window_flags |= ImGuiWindowFlags_NoScrollbar;
		//window_flags |= ImGuiWindowFlags_MenuBar;
		window_flags |= ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoResize;
		window_flags |= ImGuiWindowFlags_NoCollapse;
		window_flags |= ImGuiWindowFlags_NoBackground;

		ImVec2 button_size = ImVec2(100, 50);

		ImGui::SetNextWindowSize(ImVec2(500, 500));
		ImGui::Begin("New Game", NULL, window_flags);

		// back button
		if (ImGui::Button("Back", button_size)) {
			engine_event.type = E_TYPE::MENU_EVENT;
			engine_event.menu_data = MN_DATA::POP;
			event_handler(engine_event);
		}
		ImGui::End();
	}
};

struct LoadGameMenu : public Menu {
	LoadGameMenu(std::function<void(Event)> event_handle) : Menu(event_handle) {}

	Event engine_event;
	virtual void draw() {
		ImGuiWindowFlags window_flags = 0;
		//window_flags |= ImGuiWindowFlags_NoTitleBar;
		window_flags |= ImGuiWindowFlags_NoScrollbar;
		//window_flags |= ImGuiWindowFlags_MenuBar;
		window_flags |= ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoResize;
		window_flags |= ImGuiWindowFlags_NoCollapse;
		window_flags |= ImGuiWindowFlags_NoBackground;

		ImVec2 button_size = ImVec2(100, 50);

		ImGui::SetNextWindowSize(ImVec2(500, 500));
		ImGui::Begin("Load Game", NULL, window_flags);

		// back button
		if (ImGui::Button("Back", button_size)) {
			engine_event.type = E_TYPE::MENU_EVENT;
			engine_event.menu_data = MN_DATA::POP;
			event_handler(engine_event);
		}
		ImGui::End();
	}
};

struct MainMenu : public Menu {
	MainMenu(std::function<void(Event)> event_handle) : Menu(event_handle) {}

	Event engine_event;
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

		// new game button
		if (ImGui::Button("New", button_size)) {
			engine_event.type = E_TYPE::MENU_EVENT;
			engine_event.menu_data = MN_DATA::NEW_GAME;
			event_handler(engine_event);
		}
		// load game button
		if (ImGui::Button("Load", button_size)) {
			engine_event.type = E_TYPE::MENU_EVENT;
			engine_event.menu_data = MN_DATA::LOAD;
			event_handler(engine_event);
		}
		// exit game button
		if (ImGui::Button("Exit", button_size)) {
			engine_event.type = E_TYPE::MENU_EVENT;
			engine_event.menu_data = MN_DATA::EXIT;
			event_handler(engine_event);
		}
		ImGui::End();
	}
};