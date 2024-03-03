#pragma once
#include "imgui.h"
#include "Map.h"

/*
	Game control unit
*/
class GameControl
{
private:
	//graphics state flags
	bool game_menu_active = false;
	bool save_menu_active = false;
	bool map_creation_active = true;

	bool* back_flag = nullptr;
	void exitMenus();

	int mx;
	int my;

	//game state flags
	bool paused = true;
	bool saved = false;
	bool frozen = false;

	Map game_map;
	
	int turn = 0;

public:
	/*
		Action section
	*/
	//init game controller
	bool init(bool& game_active_flag);

	//run one iteration
	void loop();

	//get transform location from shader
	int getMapTransformLoc();

	//pause utilities
	bool getFrozen() { return frozen; };
	void Pause();
	void unPause();

	//other utilites
	void setMouse(int x, int y) {
		mx = x; my = y;
	}

	/*
		Graphics section
	*/
	//draw graphical game components: called every draw frame
	void Draw();

	void saveMenu();	//save menu
	void gameMenu();	//game menu
	void mapCreationMenu();	//map creation menu

	//save game state to file system
	void saveState();
};

