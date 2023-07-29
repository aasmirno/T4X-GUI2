#include "GameControl.h"

void GameControl::exitMenus() {
	assert(back_flag != nullptr);
	*back_flag = true;
}

void GameControl::saveState() {
	//TODO: save game
}

int GameControl::getMapTransformLoc() {
	return game_map.getTransformLoc();
}

void GameControl::loop() {
	if (!paused) {
		game_map.loop();
		turn++;
	}
}

bool GameControl::init(bool& game_active_flag) {
	back_flag = &game_active_flag;

	paused = true;
	frozen = false;


	game_map.initialise();
	return true;
}

void GameControl::Draw() {
	glClear(GL_COLOR_BUFFER_BIT);

	if (game_menu_active) {
		gameMenu();
	}
	else if (save_menu_active) {
		saveMenu();
	}
	else if (map_creation_active) {
		mapCreationMenu();
	}
}

void GameControl::saveMenu() {
	ImGui::Begin("Save");
	ImGui::Text("Game not saved");
	if(ImGui::Button("Save and Exit")) {
		saveState();
		exitMenus();
	}

	if (ImGui::Button("Exit without saving")) {
		exitMenus();
	}
	ImGui::End();

}

void GameControl::gameMenu() {
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

	if (ImGui::Button("Exit to Main Menu")) {
		save_menu_active = true;
		game_menu_active = false;
		paused = true;
	}

	ImGui::End();
}

void GameControl::Pause() {
	frozen = true;
	paused = true;
}

void GameControl::unPause() {
	frozen = false;
}

void GameControl::mapCreationMenu() {
	game_map.draw();
	game_map.drawDisplay();
	game_map.drawCreationMenu();
}