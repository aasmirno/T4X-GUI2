#pragma once
#include <vector>
#include <assert.h>
#include <random>

class TileMap {
private:
	enum TileType : uint8_t { OCEAN, BEACH, PLAIN, FOREST, BLANK1, BLANK2, MOUNTAIN, PEAK };
	enum ResourceType : uint8_t { NONE, IRON, COPPER, CHROMIUM, OIL, COAL };

	/*
		Tile class, stores tile info
	*/
	struct Tile {
		TileType type;
		ResourceType resource;
		int posx;
		int posy;

		Tile(int x, int y) : posx{ x }, posy{ y }, type{ PLAIN }, resource{NONE} {
		}

	};

	std::vector<uint8_t> tile_map_ids;	//store graphical ids
	std::vector<Tile> tile_map;	//store tile structs
	int width = 0;
	int height = 0;

public:
	//get graphical id array - type: 0-Tile textures, 1-resources
	uint8_t* getIDArray(std::vector<float>& height_map, float ocean_level, float beach_height, float mountain_height, uint8_t type) {
		assert(height_map.size() == tile_map_ids.size());

		//assign ids
		for (size_t index = 0; index < tile_map_ids.size(); index++) {
			if (type == 0) {
				float height = height_map[index];
				if (height < ocean_level) {
					tile_map[index].type = TileType::OCEAN;
					tile_map_ids[index] = TileType::OCEAN;
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

	//generate resources: x_pct - percent chance for resource type x to spawn
	//some resources have a higher chance of spawning in peaks and foothills: iron, copper, chromium
	//chromium will only spawn above some elevation
	void genResources(std::vector<float>& height_map, float iron_pct, float chr_pct, float cu_pct, float oil_pct, float coal_pct) {

		for (size_t index = 0; index < tile_map.size(); index++) {

		}
	}
};