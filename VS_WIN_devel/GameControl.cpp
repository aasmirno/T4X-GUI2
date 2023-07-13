#include "GameControl.h"

int GameControl::getMapTransformLoc() {
	return game_map.getTransformLoc();
}

void GameControl::loop() {
	if (!paused) {
		game_map.loop();
		turn++;
	}
}

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
	ImGui::Text("	Turn: %d", turn);
	if (ImGui::Button("toggle pause")) {
		paused = !paused;
	}
	ImGui::SameLine();
	ImGui::Text("%d", paused);

	ImGui::End();

}
