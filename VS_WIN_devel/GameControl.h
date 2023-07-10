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
	bool init();
	void Draw();

};

