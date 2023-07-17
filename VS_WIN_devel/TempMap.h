#pragma once
#include <vector>
#include <cmath>
#include "imgui.h"

class TempMap {
private:
	struct TempCell {
		float current_energy = 0.0f;
		float max_energy_capacity = 0.0f;
	};

	enum TempLevel : uint8_t { T1, T2, T3, T4, T5, T6, T7, T8 };
	int width;
	int height;
	float global_max;

	float added = 0.0f;
	float radiated = 0.0f;

	std::vector<TempCell> ground_map;	//ground temperature map
	std::vector<float> air_map;			//air temperature map
	std::vector<float> cloud_map;		//cloud map

	std::vector<uint8_t> ids;

	//parameters
	float cloud_reflect_percent = 0.0f;	//percentage of incoming solar energy reflected by cloud cover
	float ground_absorb_percent = 0.0f;	//percentage of incoming solar energy absorbed by ground
	float air_absorb_percent = 0.0f;	//percentage of incoming solar energy absorbed by atmosphere
	float evap_point = 0.0f;		//energy required for evaporation
	float evap_converted = 0.0f;	//percentage of energy converted to clouds
	float ground_air_rate = 0.0f;	//energy rate transfer between ground and air


public:
	//return and array of texture ids: 0 = air temp, 1 = surf temp
	uint8_t* getIDArray(uint8_t type) {
		for (size_t i = 0; i < air_map.size(); i++) {
			assignID(i, type);
		}

		return &ids[0];
	}

	void assignID(int i, uint8_t type) {
		//assign air_temp map
		if (type == 0) {
			if (air_map[i] < (1.0f / 16.0f)) {
				ids[i] = T1;
			}
			else if (air_map[i] < (2.0f / 16.0f)) {
				ids[i] = T2;
			}
			else if (air_map[i] < (3.0f / 16.0f)) {
				ids[i] = T3;
			}
			else if (air_map[i] < (4.0f / 16.0f)) {
				ids[i] = T4;
			}
			else if (air_map[i] < (5.0f / 16.0f)) {
				ids[i] = T5;
			}
			else if (air_map[i] < (6.0f / 16.0f)) {
				ids[i] = T6;
			}
			else if (air_map[i] < (7.0f / 16.0f)) {
				ids[i] = T7;
			}
			else {
				ids[i] = T8;
			}
		}
		//assign ground_temp map
		else if (type == 1) {
			if (ground_map[i].current_energy < (1.0f / 8.0f)) {
				ids[i] = T1;
			}
			else if (ground_map[i].current_energy < (2.0f / 8.0f)) {
				ids[i] = T2;
			}
			else if (ground_map[i].current_energy < (3.0f / 8.0f)) {
				ids[i] = T3;
			}
			else if (ground_map[i].current_energy < (4.0f / 8.0f)) {
				ids[i] = T4;
			}
			else if (ground_map[i].current_energy < (5.0f / 8.0f)) {
				ids[i] = T5;
			}
			else if (ground_map[i].current_energy < (6.0f / 8.0f)) {
				ids[i] = T6;
			}
			else if (ground_map[i].current_energy < (7.0f / 8.0f)) {
				ids[i] = T7;
			}
			else {
				ids[i] = T8;
			}
		}
		//assign cloud cover map
		else if (type == 2) {
			if (cloud_map[i] < (1.0f / 8.0f)) {
				ids[i] = T1;
			}
			else if (cloud_map[i] < (2.0f / 8.0f)) {
				ids[i] = T2;
			}
			else if (cloud_map[i] < (3.0f / 8.0f)) {
				ids[i] = T3;
			}
			else if (cloud_map[i] < (4.0f / 8.0f)) {
				ids[i] = T4;
			}
			else if (cloud_map[i] < (5.0f / 8.0f)) {
				ids[i] = T5;
			}
			else if (cloud_map[i] < (6.0f / 8.0f)) {
				ids[i] = T6;
			}
			else if (cloud_map[i] < (7.0f / 8.0f)) {
				ids[i] = T7;
			}
			else {
				ids[i] = T8;
			}
		}
	}

	void init(int size_x, int size_y) {
		width = size_x;
		height = size_y;
		refresh();
		getIDArray(0);
	}

	int getSize() {
		return width * height;
	}

	float getAdded() { return added; }
	float getRadiated() { return radiated; }

	float getTemp(int x, int y, uint8_t map) {
		int flat_index = y * width + x;
		if (flat_index > 0 && flat_index < ground_map.size()) {
			if (map == 0) {
				return air_map[0];
			}
			else if (map == 1) {
				return ground_map[0].current_energy;
			}
		}
	}

	void refresh() {
		//clear ids vector
		ids.clear();
		ids.resize(width * height);

		//clear air map
		air_map.clear();
		air_map.resize(width * height);

		//clear ground map
		ground_map.clear();
		ground_map.resize(width * height);
		
		//clear cloud map
		cloud_map.clear();
		cloud_map.resize(width * height);
		getIDArray(0);
	}

	//change specific heat capacities from heightmap data and ocean level
	void remapCapacities(std::vector<float> height_map, float ocean_level) {
		assert(height_map.size() == ground_map.size());
		for (int index = 0; index < height_map.size(); index++) {
			if (height_map[index] > ocean_level) {	//set land energy capacity
				ground_map[index].max_energy_capacity = 5.0f;
			}
			else { //set sea capacities based on ocean depth
				float water_depth = ocean_level - height_map[index];
				ground_map[index].max_energy_capacity = 1.0f * (water_depth + 10.0f);
			}
		}
	}

	//apply solar energy to air cells
	void solarHeat(int sun_x, int sun_y, float intensity, int drop_off) {
		added = 0.0f;
		for (int y = 0; y < height; y++) {
			for (int offset = -3; offset < 3; offset++) {
				if (sun_x + offset > -1 && sun_x + offset < width) {
					//calculate total solar energy applied at tile
					float dist_from_equator = std::abs(y - sun_y);
					float applied_heat = intensity / std::pow(std::max(dist_from_equator, 1.0f), drop_off);

					//calculate cloud reflection
					if (cloud_map[y * width + (sun_x + offset)] > 0.0f) {
						applied_heat *= (1.0f - cloud_reflect_percent);
					}

					added += applied_heat;
					//apply air energy
					air_map[y * width + (sun_x + offset)] += applied_heat * air_absorb_percent;	//add applied heat * air absorbtion rate
					applied_heat -= applied_heat * air_absorb_percent;

					//apply ground energy
					ground_map[y * width + (sun_x + offset)].current_energy += applied_heat;

				}
			}
		}
	}

	//transfer air energy to the ground
	void groundAirTransfer() {
		for (size_t index = 0; index < ground_map.size(); index++) {
			ground_map[index].current_energy -= ground_air_rate;
			air_map[index] += ground_air_rate;
		}
	}

	//radiate heat from the air and remove excess heat from ground
	void radiateHeat(float radiation_factor) {
		radiated = 0.0f;
		for (size_t index = 0; index < air_map.size(); index++) {
			float lost_heat = radiation_factor;

			//radiate air
			air_map[index] -= lost_heat;
			radiated += lost_heat;
		}
	}

	void evaporateWater(std::vector<float>& height_map, float ocean_level) {
		for (size_t index = 0; index < ground_map.size(); index++) {
			//if the tile is water and has engough energy
			if (height_map[index] < ocean_level && ground_map[index].current_energy > evap_point) {
				//calculate cloud creation
				float clouds_created = (ground_map[index].current_energy - evap_point) * evap_converted;
				cloud_map[index] += clouds_created;	//add clouds
			}
		}
	}


	void update(int sunx, int suny, float solar_intensity, float radiation_factor, int drop_off, std::vector<float> &height_map, float ocean_level) {
		//add solar heat to the air
		solarHeat(sunx, suny, solar_intensity, drop_off);

		//evaporate water
		evaporateWater(height_map, ocean_level);

		//transfer ground to air
		groundAirTransfer();

		//radiate heat air
		radiateHeat(radiation_factor);
	}

	void drawDebug() {
		ImGui::Begin("temp map debug");
		ImGui::SliderFloat("cloud_reflect_percent", &cloud_reflect_percent, 0.0f, 1.0f);
		ImGui::SliderFloat("ground_absorb_percent", &ground_absorb_percent, 0.0f, 1.0f);
		ImGui::SliderFloat("air_absorb_percent", &air_absorb_percent, 0.0f, 1.0f);
		ImGui::SliderFloat("evap_point", &evap_point, -10.0f, 10.0f);
		ImGui::SliderFloat("evap_converted", &evap_converted, 0.0f, 1.0f);
		ImGui::SliderFloat("ground_air_rate", &ground_air_rate, 0.0f, 5.0f);
		ImGui::End();
	}
};