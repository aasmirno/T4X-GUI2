#include "Menus.h"

void Menus::Draw() {
	if (draw_debug == true)
		Debug();
}

void Menus::Debug() {
	ImGui::Begin("Debug");

	ImGui::End();
}