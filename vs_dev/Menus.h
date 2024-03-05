#pragma once
#include "MenuHandler.h"
#include "EventManager.h"
#include "imgui.h"
/*
	class MenuBase
		Base class for a drawable menu
	methods
	draw()
		Draw the menu
*/
class MenuBase {
protected:
	uint32_t handle;	//menu id
	int WINDOW_W;
	int WINDOW_H;
public:
	MenuBase(int W, int H, uint32_t h) {
		WINDOW_W = W;
		WINDOW_H = H;
		handle = h;
	}

	virtual EventManager::StateEvent draw();
};

class MainMenu : public MenuBase {
public:
	MainMenu(int W, int H, uint32_t h) : MenuBase(W, H, h) {}

	virtual EventManager::StateEvent draw() {
		EventManager::StateEvent e = EventManager::StateEvent{ EventManager::WINDOW_EVENT, 0, 0 };

		ImGuiWindowFlags flags = 0;
		flags |= ImGuiWindowFlags_NoMove;
		flags |= ImGuiWindowFlags_NoResize;
		flags |= ImGuiWindowFlags_NoCollapse;

		ImGui::SetNextWindowPos(ImVec2{ (float)WINDOW_W / 2 - 200,(float)WINDOW_H / 2 - 200 });
		ImGui::SetNextWindowSize(ImVec2{ 400,400 });

		ImGui::Begin("Main Menu", NULL, flags);

		//new button
		if (ImGui::Button("New Game", ImVec2{ 400,30 })) {
			e = EventManager::StateEvent{EventManager::MENUCHANGE_EVENT, 0, 1};
		}

		//loading button
		if (ImGui::Button("Load Game", ImVec2{ 400,30 })) {

		}

		//options
		if (ImGui::Button("Options", ImVec2{ 400,30 })) {
			e = EventManager::StateEvent{EventManager::MENUCHANGE_EVENT, 0, 2};
		}

		//exit
		if (ImGui::Button("Exit", ImVec2{ 400,30 })) {
			e = EventManager::StateEvent{ EventManager::MENUCHANGE_EVENT, 0, 3 };
		}

		ImGui::End();

		return e;
	}
};

class OptionsMenu : public MenuBase {
public:
	virtual EventManager::StateEvent draw() {
		EventManager::StateEvent e = EventManager::StateEvent{ EventManager::WINDOW_EVENT, 0, 0 };

		ImGuiWindowFlags flags = 0;
		flags |= ImGuiWindowFlags_NoMove;
		flags |= ImGuiWindowFlags_NoResize;
		flags |= ImGuiWindowFlags_NoCollapse;

		ImGui::SetNextWindowPos(ImVec2{ (float)WINDOW_W / 2 - 200,(float)WINDOW_H / 2 - 200 });
		ImGui::SetNextWindowSize(ImVec2{ 400,400 });

		ImGui::Begin("Options", NULL, flags);
		//go back to main menu
		if (ImGui::Button("Back", ImVec2{ 400,30 })){

		}
		ImGui::End();
	}
};