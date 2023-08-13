#pragma once
#include <vector>
#include "BaseMap.h"


class WaterMap : BaseMap{
private:
	struct WaterTile{
		float surface_water = 0.0f;
		float outflow[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	};

	std::vector<WaterTile> water_map;

public:
	void initialise(int w, int h) {
		BaseMap::initialise(w, h);
		water_map.resize(w * h);
	}

	void iterate(std::vector<float>& height_map) {
		std::random_device rd;
		std::mt19937 mt(rd());

		std::uniform_real_distribution<> coordx(0, width - 3);
		std::uniform_real_distribution<> coordy(0, height - 3);

		addWater(water_map, mt, coordx, coordy, 1000, 0.1f);

	}

	/*
		Add water to a water map
	*/
	void addWater(std::vector<WaterTile>& water_map, 
		std::mt19937& erand, std::uniform_real_distribution<>& xrand, std::uniform_real_distribution<>& yrand,
		int num_droplets, float water_added) {

		int search_x = 0;
		int search_y = 0;

		for (int i = 0; i < num_droplets; i++) {
			search_x = (int)xrand(erand);
			search_y = (int)yrand(erand);
			if (coordCheck(search_x, search_y)) {
				water_map[search_y * width + search_x].surface_water += 0.1;
			}
		}
	}

	void update_flow(std::vector<float>& height_map, std::vector<WaterTile>& water_map) {
		for (int index = 0; index < water_map.size(); index++) {

		}
	}
};