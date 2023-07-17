#pragma once
#include <vector>
#include <assert.h>

class TileMap {
private:
	enum TileType : uint8_t { OCEAN, BEACH, PLAIN, FOREST, BLANK1, BLANK2, MOUNTAIN, PEAK };

	/*
		Tile class, stores tile info
	*/
	struct Tile {
		TileType type;
		int posx;
		int posy;

		Tile(int x, int y) : posx{ x }, posy{ y }, type{ PLAIN } {
		}

	};

	std::vector<uint8_t> tile_map_ids;	//store graphical ids
	std::vector<Tile> tile_map;	//store tile structs
	int width = 0;
	int height = 0;

public:
	//get graphical id array
	uint8_t* getIDArray(std::vector<float> &height_map, float ocean_level, float beach_height, float mountain_height) {
		assert(height_map.size() == tile_map_ids.size());

		//assign ids
		for (size_t index = 0; index < tile_map_ids.size(); index++) {
			float height = height_map[index];
			if (height < ocean_level) {
				tile_map[index].type = TileType::OCEAN;
				tile_map_ids[index] = TileType::OCEAN;
			}
			else if (height < ocean_level + beach_height) {
				tile_map[index].type = TileType::BEACH;
				tile_map_ids[index] = TileType::BEACH;
			}
			else if (height < mountain_height) {
				tile_map[index].type = TileType::PLAIN;
				tile_map_ids[index] = TileType::PLAIN;
			}
			else {
				tile_map[index].type = TileType::MOUNTAIN;
				tile_map_ids[index] = TileType::MOUNTAIN;
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


};