#include "MenuHandler.h"

void MenuHandler::draw() {
	for (auto& menu_id : active_menus) {
		menu_id->draw();
	}
}

void MenuHandler::hideMenu(uint32_t menu_id) {
	auto menu_pair = menus.find(menu_id);
	if (menu_pair != menus.end()) {
		active_menus.push_back(menus)
	}
}

void MenuHandler::showMenu(uint32_t menu_id) {

}