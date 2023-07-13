#pragma once
#include "imgui.h"
#include "Map.h"

/*
	Game control unit
*/
class GameControl
{
private:
	bool paused = true;
	Map game_map;
	int turn = 0;

public:
	void loop();
	int getMapTransformLoc();
	bool init();
	void Draw();

};

