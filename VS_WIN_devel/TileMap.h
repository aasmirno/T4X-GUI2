#pragma once
#include <vector>
#include <queue>
#include <unordered_map>
#include "FastNoiseLite.h";
#include <assert.h>
#include <random>
#include <cmath>

class TileMap {
private:
	enum TileType : uint16_t { DEEP_OCEAN, COASTAL_OCEAN, BEACH, PLAIN, FOREST, BLANK2, MOUNTAIN, PEAK };
	enum ResourceType : uint16_t { NONE, IRON, COPPER, CHROMIUM, OIL, COAL };

	/*
		Tile class, stores tile info
	*/
	struct Tile {
		TileType type;
		ResourceType resource;
		int posx;
		int posy;

		Tile(int x, int y) : posx{ x }, posy{ y }, type{ PLAIN }, resource{ NONE } {
		}

	};

	std::vector<uint16_t> tile_map_ids;	//store graphical ids
	std::vector<Tile> tile_map;	//store tile structs
	int width = 0;
	int height = 0;

	//check coordinate against bounds
	bool coordCheck(int x, int y) {
		if (x < 0 || x >= width) {
			return false;
		}
		if (y < 0 || y >= height) {
			return false;
		}
		return true;
	}

	//euclidean distance between flat indices
	float coordDist(int i1, int i2) {
		return std::sqrt(pow(i1 % width - i2 % width, 2) + pow(i1 / width - i2 / width, 2));
	}
public:
	//get graphical id array - type: 0-Tile textures, 1-resources
	uint16_t* getIDArray(std::vector<float>& height_map, float ocean_level, float beach_height, float mountain_height, uint8_t type) {
		assert(height_map.size() == tile_map_ids.size());

		//assign ids
		for (size_t index = 0; index < tile_map_ids.size(); index++) {
			if (type == 0) {
				float height = height_map[index];
				if (height < (ocean_level * (19.0f/20.0f))) {
					tile_map[index].type = TileType::DEEP_OCEAN;
					tile_map_ids[index] = TileType::DEEP_OCEAN;
				}
				else if (height < ocean_level) {
					tile_map[index].type = TileType::COASTAL_OCEAN;
					tile_map_ids[index] = TileType::COASTAL_OCEAN;
				}
				else if (height < ocean_level + beach_height) {
					tile_map[index].type = TileType::BEACH;
					tile_map_ids[index] = TileType::BEACH;
				}
				else if (height < mountain_height - 0.07) {
					tile_map[index].type = TileType::PLAIN;
					tile_map_ids[index] = TileType::PLAIN;
				}
				else if (height < mountain_height) {
					tile_map[index].type = TileType::MOUNTAIN;
					tile_map_ids[index] = TileType::MOUNTAIN;
				}
				else {
					tile_map[index].type = TileType::PEAK;
					tile_map_ids[index] = TileType::PEAK;
				}
			}
			else if (type == 1) {
				tile_map_ids[index] = tile_map[index].resource;
			}
		}
		return &tile_map_ids[0];
	}

	void initialise(int m_width, int m_height) {
		tile_map.clear();
		tile_map_ids.clear();
		width = m_width;
		height = m_height;

		tile_map_ids.resize(width * height);

		tile_map.reserve(width * height);
		for (int i = 0; i < width * height; i++) {
			tile_map.push_back(Tile{ i % width, i / width });
		}

	}

	void refresh() {
		tile_map_ids.clear();
		tile_map.clear();
		initialise(width, height);
	}

	size_t getSize() {
		assert(tile_map_ids.size() == tile_map.size());
		return tile_map.size();
	}

	//run a bernoulli trial n times (binomial)
	bool roll(float success, float times) {
		std::random_device rd;
		std::mt19937 mt(rd());
		std::uniform_real_distribution<> trial(0, 1.0);

		for (int i = 0; i < times; i++) {
			if (trial(mt) < success) {
				return true;
			}
		}
		return false;
	}

	void applySeam(ResourceType r, int index, int max_dist, FastNoiseLite& noise, std::vector<float>& height_map, float ocean_height, float mountain_height) {
		//initialise constants
		float var = 0.5f * max_dist;

		//initialise bfs ds
		std::queue<int> bfs_queue;
		std::unordered_map<int,int> visited;

		//push first object
		visited[index];
		bfs_queue.push(index);

		while (!bfs_queue.empty()) {
			int curr_index = bfs_queue.front();
			bfs_queue.pop();

			//mark as visited and set resource type
			
			tile_map[curr_index].resource = r;

			int cx = curr_index % width;
			int cy = curr_index / width;

			int x_off[4] = { -1,1,0,0 };
			int y_off[4] = { 0,0,-1,1 };
			for (int off = 0; off < 4; off++) {

				int tx = cx + x_off[off];
				int ty = cy + y_off[off];
				//float dist = coordDist(ty * width + tx, index) - (abs(noise.GetNoise((float)(tx),(float)(ty)))*var);
				if (
					coordCheck(tx, ty) &&								//in bounds
					visited.find(ty * width + tx) == visited.end() &&	//not already visited
					height_map[ty * width + tx] > ocean_height &&					//not an ocean tile
					coordDist(ty * width + tx, index) - (abs(noise.GetNoise((float)(tx), (float)(ty))) * var) < max_dist //within max distance
					) {
					visited[ty * width + tx];
					bfs_queue.push(ty * width + tx);
				}
			}
		}

	}

	//generate resources: x_pct - percent chance for resource type x to spawn
	//some resources have a higher chance of spawning in peaks and foothills: iron, copper, chromium
	//chromium will only spawn above some elevation
	void genResources(std::vector<float>& height_map, float iron_pct, float chr_pct, float cu_pct, float oil_pct, float coal_pct, float ocean_height, float mountain_height, uint8_t rm) {
		//clear resources
//#TODO: optimise with seperate resource array
		for (size_t index = 0; index < tile_map.size(); index++) {
			tile_map[index].resource = NONE;
		}

		//initialise noise
		FastNoiseLite noise;	//simplex
		noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
		noise.SetFractalType(FastNoiseLite::FractalType_Ridged);
		std::random_device rd;
		noise.SetSeed(rd());
		noise.SetFrequency(0.05);

		//patch method
		if (rm == 0) {
			for (size_t index = 0; index < tile_map.size(); index++) {
				/*if (index % 1000 > 1) {
					printf("index at: %d\n", index);
				}*/

				if (height_map[index] > ocean_height
					&& tile_map[index].resource == NONE
					) {
					if (roll(coal_pct, 1)) {
						applySeam(COAL, index, 10, noise, height_map, ocean_height, mountain_height);
					}
					else if (roll(iron_pct, 1)) {
						applySeam(IRON, index, 6, noise, height_map, ocean_height, mountain_height);
					}

				}
			}
		}

		printf("Done\n");
	}
};