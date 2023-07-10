#include "GameControl.h"

bool GameControl::init() {
	game_map.initialise();
	return true;
}

void GameControl::Draw() {
	glClear(GL_COLOR_BUFFER_BIT);

	game_map.draw();

	ImGuiWindowFlags flags = 0;
	flags |= ImGuiWindowFlags_NoMove;
	flags |= ImGuiWindowFlags_NoResize;

	ImGui::SetNextWindowPos(ImVec2{ 10,10 });
	ImGui::SetNextWindowSize(ImVec2{ 300,600 });

	ImGui::Begin("Game", NULL, flags);

	ImGui::End();

}
