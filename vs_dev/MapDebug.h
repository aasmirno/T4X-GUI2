#pragma once
#include "imgui.h"
#include <string>

class BaseMenu {
private:
	std::string name;
	size_t size_x;
	size_t size_y;
	ImGui

public:
	BaseMenu();
};

class OptionsMenu {
public:
	OptionsMenu

	void draw() {
		ImGui::Begin();

		ImGui::End();
	}
}