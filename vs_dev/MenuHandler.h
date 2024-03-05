#pragma once
#include <unordered_map>
#include <memory>
#include "EventManager.h"
#include "Menus.h"

/*
	class Menu handler
		Draws imgui based menus
	methods:
	draw()
		update drawing flags from event handler and draw menus
*/
class MenuHandler {
private:
	void (*onEvent)(EventManager::StateEvent) = nullptr;

	std::vector<std::unique_ptr<MenuBase>> active_menus;
	std::unordered_map<uint32_t, std::unique_ptr<MenuBase>> menus;	//list of menus and handles

	//menu flags;
	bool main_menu = false;
	bool general_debug_menu = false;
	bool game_menu = false;

public:
	//event manager pointer, window width, window height
	MenuHandler(void (*f)(EventManager::StateEvent), int w, int h) {
		//set event manager
		onEvent = f;

		//insert the main menu
		std::pair<uint32_t, std::unique_ptr<MenuBase>> main_menu = { 1, std::unique_ptr<MenuBase>(new MainMenu(w, h, 1)) };
		menus.emplace(main_menu);
		active_menus.push_back(main_menu.second);
	}

	/*
		Drawing methods
	*/
	//draw the active menus
	void draw();

	void showMenu(uint32_t menu_id);

	void hideMenu(uint32_t menu_id);
};

