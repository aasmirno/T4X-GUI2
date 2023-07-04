#pragma once
#include "imgui.h"

//imgui debug menu
class Menus {
private:
	bool draw_debug = false;

	void Debug();
public:
	void Draw();
};