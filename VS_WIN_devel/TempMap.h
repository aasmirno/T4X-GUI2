#pragma once
#include <vector>
#include <cmath>
#include <chrono>
#include <FastNoise/FastNoise.h>

#include "imgui.h"
#include "FastNoiseLite.h"

class TempMap : public BaseMap {
private:
	enum Direction : uint8_t { UL, U, UR, L, R, DL, D, DR };
	enum TempLevel : uint16_t { T1, T2, T3, T4, T5, T6, T7, T8 };

	/*
		Temperature cell
			current_energy: current energy in this cell
			max_energy_capacity: max energy that can be stored
			velocity: speed of motion
			direction: direction of motion
				0 1 2
				3 C 4
				5 6 7
	*/
	struct TempCell {
		float current_energy = 0.0f;
		bool decreasing = false;
	};

	/*
		Cloud Cell
			moisture: amount of moisture in cloud
	*/
	struct CloudCell {
		float moisture;
		void addMoisture(float new_moisture) {
			moisture += moisture + new_moisture < 1.0f ? new_moisture : 1.0f - new_moisture;
		}
	};

	/*
		Meta data
	*/
	int iterations = 0;

	float added = 0.0f;
	float radiated = 0.0f;

	float times[7] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

	/*
		debug parameters
	*/
	float temp_graph[1000];
	float temp_graph2[1000];
	int temp_graph_index = 0;
	float min_t = 0.0f;
	float max_t = 0.0f;
	float last_min = 0.0f;
	float last_max = 0.0f;
	int counter_to_max = 0;
	int counter_to_min = 0;

	/*
		data vectors
	*/
	std::vector<TempCell> ground_map;	//ground cell map
	std::vector<CloudCell> cloud_map;	//cloud map

	std::vector<uint16_t> ids;

	//parameters
	float cloud_reflect_percent = 0.0f;	//percentage of incoming solar energy reflected by cloud cover
	float ground_absorb_percent = 0.0f;	//percentage of incoming solar energy absorbed by ground
	float evap_point = 0.0f;		//energy required for evaporation
	float evap_converted = 0.0f;	//percentage of energy converted to clouds
	float transport_rate = 0.2f;	//percentage of moisture transferred by noise wind
	float dispersion_rate = 0.1f;	//rate of cloud dispersion
	int sun_x_rad = 0;	//sun radius x
	int sun_y_rad = 0;	//sun radius y

public:
	//return and array of texture ids: 0 = air temp, 1 = surf temp
	uint16_t* getIDArray(uint8_t type) {
		for (size_t i = 0; i < ground_map.size(); i++) {
			//assign ground_temp map
			if (type == 1) {
				if (ground_map[i].current_energy < (1.0f / 8.0f) * 30.0f) {
					ids[i] = T1;
				}
				else if (ground_map[i].current_energy < (2.0f / 8.0f) * 30.0f) {
					ids[i] = T2;
				}
				else if (ground_map[i].current_energy < (3.0f / 8.0f) * 30.0f) {
					ids[i] = T3;
				}
				else if (ground_map[i].current_energy < (4.0f / 8.0f) * 30.0f) {
					ids[i] = T4;
				}
				else if (ground_map[i].current_energy < (5.0f / 8.0f) * 30.0f) {
					ids[i] = T5;
				}
				else if (ground_map[i].current_energy < (6.0f / 8.0f) * 30.0f) {
					ids[i] = T6;
				}
				else if (ground_map[i].current_energy < (7.0f / 8.0f) * 30.0f) {
					ids[i] = T7;
				}
				else {
					ids[i] = T8;
				}
			}
			//assign cloud cover map
			else if (type == 2) {
				if (cloud_map[i].moisture < (1.0f / 8.0f)) {
					ids[i] = T1;
				}
				else if (cloud_map[i].moisture < (2.0f / 8.0f)) {
					ids[i] = T2;
				}
				else if (cloud_map[i].moisture < (3.0f / 8.0f)) {
					ids[i] = T3;
				}
				else if (cloud_map[i].moisture < (4.0f / 8.0f)) {
					ids[i] = T4;
				}
				else if (cloud_map[i].moisture < (5.0f / 8.0f)) {
					ids[i] = T5;
				}
				else if (cloud_map[i].moisture < (6.0f / 8.0f)) {
					ids[i] = T6;
				}
				else if (cloud_map[i].moisture < (7.0f / 8.0f)) {
					ids[i] = T7;
				}
				else {
					ids[i] = T8;
				}
			}
		}

		return &ids[0];
	}

	//initialise map
	void initialise(int width, int height) {
		BaseMap::initialise(width, height);
		refresh();
		getIDArray(0);
	}

	float getAdded() { return added; }
	float getRadiated() { return radiated; }

	float getTemp(int x, int y, uint8_t map) {
		int flat_index = y * width + x;
		if (flat_index > 0 && flat_index < ground_map.size()) {
			return ground_map[0].current_energy;
		}
	}

	void refresh() {
		//reset temp graph and data
		for (int i = 0; i < 1000; i++) {
			temp_graph[i] = 0.0f;
		}
		for (int i = 0; i < 1000; i++) {
			temp_graph2[i] = 0.0f;
		}
		temp_graph_index = 0;

		min_t = 0.0f;
		max_t = 0.0f;
		counter_to_max = 0;
		counter_to_min = 0;

		//clear ids vector
		ids.clear();
		ids.resize(width * height);

		//clear ground map
		ground_map.clear();
		ground_map.resize(width * height);

		//clear cloud map
		cloud_map.clear();
		cloud_map.resize(width * height);

		//assign id array
		getIDArray(0);
	}

	/*
		Simulation section - functions pertaining to climate simulation
	*/

	//apply solar energy to air cells
	void solarHeat(int sun_x, int sun_y, float intensity, int x_rad, int y_rad, float rad_fac) {
		added = 0.0f;
		for (size_t index = 0; index < ground_map.size(); index++) {
			int pos_x = index % width;
			int pos_y = index / width;

			if (pos_x > sun_x - x_rad && pos_x < sun_x + x_rad && pos_y > sun_y - y_rad && pos_y < sun_y + y_rad) {
				//calculate total solar energy applied at tile
				float dist_from_center = std::sqrt(std::pow(pos_y - sun_y, 2) + std::pow(pos_x - sun_x, 2));

				float applied_heat = intensity / std::max(dist_from_center, 1.0f);

				//calculate cloud reflection
				if (cloud_map[pos_y * width + pos_x].moisture > 0.0f) {
					applied_heat *= (1.0f - cloud_reflect_percent);
				}

				added += applied_heat;

				//apply ground energy
				ground_map[pos_y * width + pos_x].current_energy += applied_heat * ground_absorb_percent;

			}
			else {
				ground_map[index].decreasing = true;
			}
		}
	}

	//radiate heat from the ground and disperse clouds
	void radiateHeat(float radiation_factor, int sun_x) {
		radiated = 0.0f;
		for (int index = 0; index < static_cast<int> (ground_map.size()); index++) {
			if (ground_map[index].decreasing) {
				int ticks_to_noon = 0;
				ticks_to_noon = sun_x > (index % width) ? sun_x - index % width + (2 * width) : (index % width - sun_x);

				ground_map[index].current_energy -= ground_map[index].current_energy / std::max(ticks_to_noon, 1);
			}

			if (cloud_map[index].moisture > 0.0f) {
				cloud_map[index].moisture -= dispersion_rate;
			}
		}
	}

	//move clouds along generated noise paths
	void moveClouds() {
		auto fnPerlin = FastNoise::New<FastNoise::OpenSimplex2S>();
		auto fnFractal = FastNoise::New<FastNoise::FractalFBm>();
		fnFractal->SetSource(fnPerlin);
		fnFractal->SetOctaveCount(9);
		float freq = 0.005;
		for (size_t index = 0; index < cloud_map.size(); index++) {
			if (cloud_map[index].moisture > 0.0f) {
				int x = index % width;
				int y = index / width;

				float test = fnFractal->GenSingle3D(freq * (float)(index % width), freq * (float)(index / width), freq * iterations, 1337) * 9;
				uint8_t d = (int)test;
				Direction dir = static_cast<Direction>(d);

				switch (dir) {
				case UL:
					x -= 1;
					y -= 1;
					break;
				case U:
					y -= 1;
					break;
				case UR:
					x += 1;
					y -= 1;
					break;
				case L:
					x -= 1;
					break;
				case R:
					x += 1;
					break;
				case DL:
					x -= 1;
					y += 1;
					break;
				case D:
					y += 1;
					break;
				case DR:
					x += 1;
					y += 1;
					break;
				}


				if (x > -1 && x < width && y > -1 && y < height) {
					cloud_map[y * width + x].moisture += cloud_map[index].moisture * transport_rate;
					cloud_map[index].moisture -= cloud_map[index].moisture * transport_rate;
				}

			}
		}
	}

	//evaporate water
	void evaporateWater(std::vector<float>& height_map, float ocean_level) {
		for (size_t index = 0; index < ground_map.size(); index++) {
			//if the tile is water and has engough energy
			if (height_map[index] < ocean_level && ground_map[index].current_energy > evap_point) {
				//calculate cloud creation
				float clouds_created = (ground_map[index].current_energy - evap_point) * evap_converted;
				cloud_map[index].moisture += clouds_created;	//add clouds
				//ground_map[index].current_energy -= (ground_map[index].current_energy - evap_point);
			}
		}
	}

	//simulation general control
	float* update(int sunx, int suny, float solar_intensity, float radiation_factor, int drop_off, std::vector<float>& height_map, float ocean_level) {
		auto begin = std::chrono::high_resolution_clock::now();
		auto end = std::chrono::high_resolution_clock::now();
		times[0] = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

		begin = std::chrono::high_resolution_clock::now();
		//add solar heat to the system
		solarHeat(sunx, suny, solar_intensity, sun_x_rad, sun_y_rad, radiation_factor);
		end = std::chrono::high_resolution_clock::now();
		times[1] = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

		begin = std::chrono::high_resolution_clock::now();
		//evaporate water
		evaporateWater(height_map, ocean_level);
		end = std::chrono::high_resolution_clock::now();
		times[2] = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

		begin = std::chrono::high_resolution_clock::now();
		//move clouds
		moveClouds();
		end = std::chrono::high_resolution_clock::now();
		times[3] = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

		begin = std::chrono::high_resolution_clock::now();
		//radiate heat air
		radiateHeat(radiation_factor, sunx);
		end = std::chrono::high_resolution_clock::now();
		times[5] = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

		//update temperature tracker
		//temp_graph[temp_graph_index] = ground_map[(height / 2) * width + 50].current_energy;
		//temp_graph2[temp_graph_index] = ground_map[51].current_energy;
		temp_graph_index++;
		if (temp_graph_index >= 1000) {
			temp_graph_index = 0;
		}
		iterations++;
		/*
		if (ground_map[(height / 2) * width + 50].current_energy < min_t) {
			min_t = ground_map[(height / 2) * width + 50].current_energy;
			counter_to_min++;
		}
		if (ground_map[(height / 2) * width + 50].current_energy > max_t) {
			max_t = ground_map[(height / 2) * width + 50].current_energy;
			min_t = max_t;
			counter_to_max++;
			counter_to_min = 0;
		}
		*/
		return times;
	}

	//imgui debug controls
	void drawDebug() {
		ImGui::Begin("temp map debug");
		ImGui::SliderFloat("cloud_reflect_percent", &cloud_reflect_percent, 0.0f, 1.0f);
		ImGui::SliderFloat("ground_absorb_percent", &ground_absorb_percent, 0.0f, 1.0f);
		ImGui::SliderFloat("evap_point", &evap_point, -10.0f, 50.0f);
		ImGui::SliderFloat("evap_converted", &evap_converted, 0.0f, 1.0f);
		ImGui::SliderFloat("transport_rate", &transport_rate, 0.0f, 1.0f);
		ImGui::SliderFloat("dispersion_rate", &dispersion_rate, 0.0f, 1.0f);
		ImGui::SliderInt("sun_x_rad", &sun_x_rad, 0, width / 2);
		ImGui::SliderInt("sun_y_rad", &sun_y_rad, 0, height / 2);
		ImGui::Separator();
		ImGui::PlotLines("equator temp", temp_graph, IM_ARRAYSIZE(temp_graph), 0, 0, -500.0f, 500.0f, ImVec2(0, 80.0f));
		ImGui::PlotLines("equator temp", temp_graph2, IM_ARRAYSIZE(temp_graph2), 0, 0, -300.0f, 300.0f, ImVec2(0, 80.0f));
		//ImGui::Text("g temp: %f at (50, %d)", ground_map[(height / 2) * width + 50].current_energy, (height / 2));
		ImGui::Text("min temp: %f, max temp: %f", min_t, max_t);
		ImGui::Text("ctmx: %d, ctmn: %d, diff: %f, diff: %f", counter_to_max, counter_to_min, max_t - min_t, abs(max_t - (max_t - min_t)));
		ImGui::End();
	}
};