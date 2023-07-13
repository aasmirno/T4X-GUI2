#pragma once
#include <vector>

class TempMap {
private:
	enum TempLevel : uint8_t { T1, T2, T3, T4, T5, T6, T7, T8 };
	int width;
	int height;
	int solar_radius = 30;
	std::vector<float> temp_map;
	std::vector<uint8_t> ids;
public:
	uint8_t* getIDArray() {
		for (size_t i = 0; i < temp_map.size(); i++) {
			assignID(i);
		}

		return &ids[0];
	}

	void assignID(int i) {
		if (temp_map[i] < (1.0f / 8.0f)) {
			ids[i] = T1;
		}
		else if (temp_map[i] < (2.0f / 8.0f)) {
			ids[i] = T2;
		}
		else if (temp_map[i] < (3.0f / 8.0f)) {
			ids[i] = T3;
		}
		else if (temp_map[i] < (4.0f / 8.0f)) {
			ids[i] = T4;
		}
		else if (temp_map[i] < (5.0f / 8.0f)) {
			ids[i] = T5;
		}
		else if (temp_map[i] < (6.0f / 8.0f)) {
			ids[i] = T6;
		}
		else if (temp_map[i] < (7.0f / 8.0f)) {
			ids[i] = T7;
		}
		else{
			ids[i] = T8;
		}
	}

	void init(int size_x, int size_y) {
		width = size_x;
		height = size_y;
		temp_map.resize(width* height);
		ids.resize(width* height);
		getIDArray();
	}

	int getSize() {
		return width * height;
	}

	void refresh() {
		temp_map.clear();
		temp_map.resize(width * height);
		for (size_t i = 0; i < temp_map.size(); i++) {
			assignID(i);
		}
	}

	void update(int sunx, int suny) {
		//apply solar heat
		for (int y = suny - solar_radius; y < suny + solar_radius; y++) {
			for (int x = sunx - solar_radius; x < sunx + solar_radius; x++) {
				if (x >= 0 && x < width && y >= 0 && y < height) {
					temp_map[y * height + x] += 0.1;
					assignID(y * height + x);
				}
			}

		}

	}


};