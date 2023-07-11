#pragma once
#include "imgui.h"
#include "Map.h"

/*
	Game control unit
*/
class GameControl
{
private:
	Map game_map;

public:
	int getMapTransformLoc();
	bool init();
	void Draw();

};

