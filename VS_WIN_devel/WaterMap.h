#pragma once
#include <vector>
#include "BaseMap.h"


class WaterMap : public BaseMap {
private:
	struct WaterTile {
		float velocity_horizontal = 0.0f;
		float velocity_vertical = 0.0f;
		float surface_water = 0.0f;
		float suspended_sediment = 0.0f;	//s1
		float sediment_new = 0.0f;			//s2
		//R, L, D, U, UL, UR, DR, DL
		float outflow[8] = { 0.0f, 0.0f, 0.0f, 0.0f,
							0.0f, 0.0f, 0.0f, 0.0f };
	};

	struct Angle {
		float x_angle = 0.0f;
		float y_angle = 0.0f;
	};

	std::vector<WaterTile> water_map;
	std::vector<uint16_t> id_array;

	float dT = 0.5f;	//time increment
	float A = 1.0f;		//cross sectional area of connecting pipes
	float g = 9.8;		//gravity constant
	float l = 1.0f;		//pipe length
	float lX = 5.0f;	//grid x distance
	float lY = 5.0f;	//grid y distance

	float Kc = 0.1f;				//sediment capacity constant
	float Ks = 0.1f;				//dissolving constant
	float Kd = 0.5f;				//deposition constant
	float min_tilt_angle = 0.0f;	//minimum terrain tilt angle
	std::vector<Angle> tilt_angles;		//precalculated terrain tilt angles

	//debug
	float total_inflow_turn = 0.0f;
	float total_outflow_turn = 0.0f;

public:
	/*
		Graphics getters
	*/
	uint16_t* getIDArray() {
		for (int index = 0; index < water_map.size(); index++) {
			float water_level = water_map[index].surface_water;
			if (water_level < 1.0f / 16) {
				id_array[index] = 0;
			}
			else if (water_level < 2.0f / 16) {
				id_array[index] = 1;
			}
			else if (water_level < 3.0f / 16) {
				id_array[index] = 2;
			}
			else if (water_level < 4.0f / 16) {
				id_array[index] = 3;
			}
			else if (water_level < 5.0f / 16) {
				id_array[index] = 4;
			}
			else if (water_level < 6.0f / 16) {
				id_array[index] = 5;
			}
			else if (water_level < 7.0f / 16) {
				id_array[index] = 6;
			}
			else if (water_level < 8.0f / 16) {
				id_array[index] = 7;
			}
			else if (water_level < 9.0f / 16) {
				id_array[index] = 8;
			}
			else if (water_level < 10.0f / 16) {
				id_array[index] = 9;
			}
			else if (water_level < 11.0f / 16) {
				id_array[index] = 10;
			}
			else if (water_level < 12.0f / 16) {
				id_array[index] = 11;
			}
			else if (water_level < 13.0f / 16) {
				id_array[index] = 12;
			}
			else if (water_level < 14.0f / 16) {
				id_array[index] = 13;
			}
			else if (water_level < 15.0f / 16) {
				id_array[index] = 14;
			}
			else {
				id_array[index] = 16;
			}
		}
		return &id_array[0];
	}
	uint16_t* getVelocityField() {
		for (int index = 0; index < water_map.size(); index++) {
			if (water_map[index].velocity_horizontal > 0.0f && water_map[index].velocity_vertical > 0.0f) {
				id_array[index] = 7;
			}
			else if (water_map[index].velocity_horizontal < 0.0f && water_map[index].velocity_vertical > 0.0f) {
				id_array[index] = 5;
			}
			else if (water_map[index].velocity_horizontal < 0.0f && water_map[index].velocity_vertical < 0.0f) {
				id_array[index] = 0;
			}
			else if (water_map[index].velocity_horizontal > 0.0f && water_map[index].velocity_vertical < 0.0f) {
				id_array[index] = 2;
			}
			else {
				id_array[index] = 3;
			}
		}
		return &id_array[0];
	}

	/*
		Debug getters
	*/
	float getWaterAt(int x, int y) {
		if (coordCheck(x, y)) {
			return water_map[y * width + x].surface_water;
		}
		return 0.0f;
	}
	std::pair<float, float> getVelocityAt(int x, int y) {
		if (coordCheck(x, y)) {
			return { water_map[y * width + x].velocity_horizontal, water_map[y * width + x].velocity_vertical };
		}
		return { 0.0f,0.0f };
	}
	std::pair<float, float> getTiltAngleAt(int x, int y) {
		if (coordCheck(x, y)) {
			return { tilt_angles[y * width + x].x_angle,  tilt_angles[y * width + x].y_angle };
		}
		return { 0.0f,0.0f };
	}

	/*
		Water data getters
	*/
	float totalWater() {
		float twater = 0.0f;
		for (auto& tile : water_map) {
			twater += tile.surface_water;
		}
		return twater;
	}
	float getTotalIn() { return total_inflow_turn; }
	float getTotalOut() { return total_outflow_turn; }

	void initialise(int w, int h) {
		BaseMap::initialise(w, h);
		water_map.resize(w * h);
		id_array.resize(w * h);
		tilt_angles.resize(w * h);
	}

	void reset() {
		water_map.clear();
		id_array.clear();

		water_map.resize(width * height);
		id_array.resize(width * height);
	}

	/*
		Perform one entire iteration of erosion algorithm
	*/
	void iterate(std::vector<float>& height_map, float ocean_level, float args[10]) {
		dT = args[0];
		A = args[1];
		g = args[2];
		l = args[3];
		lX = args[4];
		lY = args[5];

		Kc = args[6];
		Ks = args[7];
		Kd = args[8];
		min_tilt_angle = args[9];

		std::random_device rd;
		std::mt19937 mt(rd());

		std::uniform_real_distribution<> coordx(0, width - 3);
		std::uniform_real_distribution<> coordy(0, height - 3);

		//addWater(water_map, height_map, mt, coordx, coordy, 10000, 0.1f, ocean_level);
		updateFlow(height_map, water_map, ocean_level);
		updateWater(water_map, height_map, ocean_level);
		moveWater(height_map, ocean_level);
		calculateVelocityField(water_map, height_map, ocean_level);
		updateErosion(height_map, ocean_level, water_map);
		moveSediment(height_map, ocean_level, water_map);
	}

	/*
		update water without adding additional water
	*/
	void moveWater(std::vector<float>& height_map, float ocean_level) {
		updateFlow(height_map, water_map, ocean_level);
		updateWater(water_map, height_map, ocean_level);
		calculateVelocityField(water_map, height_map, ocean_level);
	}

	/*
		Add water using random droplet method
		#TODO add river sources
	*/
	void addWaterOnly(std::vector<float>& height_map, float ocean_level) {
		std::random_device rd;
		std::mt19937 mt(rd());

		std::uniform_real_distribution<> coordx(0, width - 3);
		std::uniform_real_distribution<> coordy(0, height - 3);
		addWater(water_map, height_map, mt, coordx, coordy, height * width * 0.25f , 0.5f, ocean_level);
	}
	void addWater(std::vector<WaterTile>& water_map, std::vector<float>& height_map,
		std::mt19937& erand, std::uniform_real_distribution<>& xrand, std::uniform_real_distribution<>& yrand,
		int num_droplets, float water_added,
		float ocean_level) {

		int search_x = 0;
		int search_y = 0;

		for (int i = 0; i < num_droplets; i++) {
			search_x = (int)xrand(erand);
			search_y = (int)yrand(erand);
			if (coordCheck(search_x, search_y) && height_map[search_y * width + search_x] > ocean_level) {
				water_map[search_y * width + search_x].surface_water += water_added;
			}
		}
	}

	/*
		Update flow rate information for each cell
	*/
	void updateFlow(std::vector<float>& height_map, std::vector<WaterTile>& water_map, float ocean_level) {
		for (int index = 0; index < water_map.size(); index++) {
			if (height_map[index] > ocean_level) {
				int curr_x = index % width;
				int curr_y = index / width;

				//iterate through neighbors
				//i=0=R, i=1=L, i=2=D, i=3=U, i=4=UL, i=5=UR, i=6=DR, i=7=DL
				int x_off[] = { 1, -1, 0, 0,  -1, 1, 1, -1 };
				int y_off[] = { 0, 0, 1, -1,  -1, -1, 1,  1 };
				for (int i = 0; i < 4; i++) {
					int neighbor_index = (curr_y + y_off[i]) * width + (curr_x + x_off[i]);
					if (coordCheck(neighbor_index)) {
						//calculate outflow
						float delta_h = height_map[index] + water_map[index].surface_water - height_map[neighbor_index] - water_map[neighbor_index].surface_water;	//height diff
						float flow_frac = (g * delta_h) / l;
						float flow_unmaxed = water_map[index].outflow[i] + (dT * A * flow_frac);
						water_map[index].outflow[i] = std::max(0.0f, flow_unmaxed);
					}
				}

				//calculate flow factor
				float factor_denom = 0.0f;
				for (auto& flow : water_map[index].outflow) {
					factor_denom += flow;
				}

				//scale flow if it exists
				if (factor_denom != 0.0f) {
					float flow_factor = std::min(1.0f, (water_map[index].surface_water * lX * lY) / (factor_denom * dT));
					for (auto& flow : water_map[index].outflow) {
						flow *= flow_factor;
					}
				}
			}
		}
	}

	/*
		Update water levels using flow rate information
	*/
	void updateWater(std::vector<WaterTile>& water_map, std::vector<float>& height_map, float ocean_level) {
		total_outflow_turn = 0.0f; total_inflow_turn = 0.0f;
		for (int index = 0; index < water_map.size(); index++) {
			if (height_map[index] > ocean_level) {
				int curr_x = index % width;
				int curr_y = index / width;

				//iterate neighbors to calculate inflow to tile
				float total_inflow = 0.0f;
				//i=0=R, i=1=L, i=2=D, i=3=U, i=4=UL, i=5=UR, i=6=DR, i=7=DL
				int x_off[] = { 1, -1, 0, 0,  -1, 1, 1, -1 };
				int y_off[] = { 0, 0, 1, -1,  -1, -1, 1,  1 };
				for (int i = 0; i < 4; i++) {
					int neighbor_index = (curr_y + y_off[i]) * width + (curr_x + x_off[i]);
					if (coordCheck(curr_y + y_off[i], curr_x + x_off[i]) && height_map[neighbor_index] > ocean_level) {
						int opp_index = 1;
						switch (i) {
						case 1:
							opp_index = 0; break;
						case 2:
							opp_index = 3; break;
						case 3:
							opp_index = 2; break;
						case 4:
							opp_index = 6; break;
						case 5:
							opp_index = 7; break;
						case 6:
							opp_index = 4; break;
						case 7:
							opp_index = 5; break;
						}

						total_inflow += water_map[neighbor_index].outflow[opp_index];
					}
				}

				//calculate outflow from tile
				float total_outflow = 0.0f;
				for (auto& rate : water_map[index].outflow) {
					total_outflow += rate;
				}

				total_inflow_turn += total_inflow;
				total_outflow_turn += total_outflow;
				//change water level accordingly
				float delta_v = dT * (total_inflow - total_outflow);
				water_map[index].surface_water += delta_v / (lX * lY);
			}
		}
	}

	/*
		Calculate velocity field
	*/
	void calculateVelocityField(std::vector<WaterTile>& water_map, std::vector<float>& height_map, float ocean_level) {
		for (int index = 0; index < water_map.size(); index++) {
			//calculate horizontal velocity
				//add left and right outflow
			float velocity_denom = -(water_map[index].outflow[1]) + water_map[index].outflow[0];

			//add right neighbor inflow if possible
			if (coordCheck((index % width) - 1, index / width)) {
				velocity_denom += water_map[(index / width) * width + ((index % width) - 1)].outflow[0];
			}
			//add left neighbor inflow if possible
			if (coordCheck((index % width) + 1, index / width)) {
				velocity_denom -= water_map[(index / width) * width + ((index % width) + 1)].outflow[1];
			}
			water_map[index].velocity_horizontal = velocity_denom / 2;

			//calculate vertical velocity
			float velocity_denom_y = -(water_map[index].outflow[3]) + water_map[index].outflow[2];
			//add upper neighbor inflow if possible
			if (coordCheck(index % width, (index / width) - 1)) {
				velocity_denom_y += water_map[((index / width) - 1) * width + index % width].outflow[2];
			}
			//add lower neighbor inflow if possible
			if (coordCheck(index % width, (index / width) + 1)) {
				velocity_denom_y += water_map[((index / width) + 1) * width + index % width].outflow[3];
			}
			water_map[index].velocity_vertical = velocity_denom_y / 2;


		}
	}

	/*
		Calculate erosion value
	*/
	void updateErosionExterior(std::vector<float>& height_map, float ocean_level) {
		updateErosion(height_map, ocean_level, water_map);
	}
	void updateErosion(std::vector<float>& height_map, float ocean_level, std::vector<WaterTile>& water_map) {
		generateTiltAngle(height_map, ocean_level, tilt_angles);

		for (int index = 0; index < water_map.size(); index++) {
			if (height_map[index] > ocean_level) {
				//calculate suspension capacity
				float C = Kc 
					* std::sin(std::sqrt(std::pow(tilt_angles[index].x_angle, 2) + std::pow(tilt_angles[index].y_angle,2))) 
					* std::sqrt(std::pow(water_map[index].velocity_horizontal, 2.0f) + std::pow(water_map[index].velocity_vertical, 2.0f));

				//dissolve or deposit based on suspension capacity
				if (C > water_map[index].suspended_sediment) {
					float eroded = Ks * (C - water_map[index].suspended_sediment);
					//erode
					if (height_map[index] - eroded < ocean_level) {
						eroded = height_map[index] - ocean_level;
					}

					height_map[index] -= eroded;
					water_map[index].suspended_sediment += eroded;
				}
				else {
					//deposit
					height_map[index] += Kd * (water_map[index].suspended_sediment - C);
					water_map[index].suspended_sediment -= Kd * (water_map[index].suspended_sediment - C);
				}

			}
		}
	}

	/*
		Generate tilt angle map based on heightmap
	*/
	void genTilt(std::vector<float>& height_map, float ocean_level) { generateTiltAngle(height_map, ocean_level, tilt_angles); }
	void generateTiltAngle(std::vector<float>& height_map, float ocean_level, std::vector<Angle>& tilt_angles) {
		tilt_angles.clear();
		tilt_angles.resize(height_map.size());

		for (int index = 0; index < height_map.size(); index++) {
			if (height_map[index] > ocean_level) {
				int curr_x = index % width;
				int curr_y = index / width;

				//calculate x component angle
				float height_left_x = coordCheck(curr_x - 1, curr_y) ? height_map[index - 1] : height_map[index];
				float height_right_x = coordCheck(curr_x + 1, curr_y) ? height_map[index + 1] : height_map[index];
				tilt_angles[index].x_angle = std::atan2(abs(height_right_x - height_left_x), 3.0f);

				//calculate x component angle
				float height_left_y = coordCheck(curr_x, curr_y - 1) ? height_map[index - width] : height_map[index];
				float height_right_y = coordCheck(curr_x, curr_y + 1) ? height_map[index + width] : height_map[index];
				tilt_angles[index].y_angle = std::atan2(abs(height_right_y - height_left_y), 3.0f);
			}
		}
	}

	/*
		Move sediment
	*/
	void moveSediment(std::vector<float>& height_map, float ocean_level, std::vector<WaterTile>& water_map) {
		for (int index = 0; index < height_map.size(); index++) {
			if (height_map[index] > ocean_level) {
				//calculate previous tile
				int x = (index % width) - (water_map[index].velocity_horizontal * dT);
				int y = (index / width) - (water_map[index].velocity_vertical * dT);

				if (coordCheck(x, y)) {
					water_map[index].sediment_new = water_map[y * width + x].suspended_sediment;
				}
			}
		}

		for (auto& tile : water_map) {
			tile.suspended_sediment = tile.sediment_new;
		}
	}
};