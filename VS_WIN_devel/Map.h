#pragma once
#include "ShaderManager.h"

#include "imgui.h"
#include <random>
#include <IL/il.h>
#include <assert.h>
#include <cstdlib>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <vector>

class MapShader :
	public ShaderManager
{
private:
	const GLint in_tile = 0;
public:
	bool init() {
		program_id = glCreateProgram();
		if (loadFromFile("D:\\Software and Tools\\C++\\T4x\\VS_WIN_devel\\resources\\mapvert.glvs", GL_VERTEX_SHADER) != true) {
			return false;
		}
		if (loadFromFile("D:\\Software and Tools\\C++\\T4x\\VS_WIN_devel\\resources\\mapfrag.glfs", GL_FRAGMENT_SHADER) != true) {
			return false;
		}
		if (loadFromFile("D:\\Software and Tools\\C++\\T4x\\VS_WIN_devel\\resources\\mapgeom.glgs", GL_GEOMETRY_SHADER) != true) {
			return false;
		}

		if (!linkProgram()) {
			printf("program linkage error\n");
			return false;
		}

		printf("shader init success\n");
		return true;
	}
};

enum TileType : uint8_t { PLAIN, MOUNTAIN, DESERT , RIVER};
enum ElevationType : uint8_t {L1, L2, L3, L4, L5, L6, L7};

/*
	Tile class, stores tile info
*/
struct Tile {
	TileType type;
	int posx;
	int posy;

	Tile(int x, int y) : posx{ x }, posy{ y }, type{PLAIN} {
	}
};

/*
	Tile map struct, holds tiles and ids
*/
class TileMap {
private:
	int width = 9;
	int height = 9;
	bool initialised = false;
	std::vector<Tile> tile_data;
	std::vector<uint8_t> tile_ids;
public:
	void initialise(int width, int height) {
		this->width = width;
		this->height = height;
		tile_data.reserve(width * height);
		tile_ids.reserve(width * height);

		//init tiles randomly
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				tile_data.push_back(Tile{ x,y });
				tile_ids.push_back(rand() % 4);
			}
		}

		initialised = true;
	}

	/*
		set a value at position x,y : zero indexed
	*/
	void set(int x, int y, Tile t) {
		assert(initialised == true);
		assert(x > -1 && x < width);
		assert(y > -1 && y < height);

		int flat_index = x + y * width;	//get flat index from x,y coords

		//update data and ids
		tile_data[flat_index] = t;
		tile_ids[flat_index] = t.type;
	}

	/*
		set a tile type at position x,y : zero indexed
	*/
	void set(int x, int y, ElevationType type) {
		assert(initialised == true);
		assert(x > -1 && x < width);
		assert(y > -1 && y < height);

		int flat_index = x + y * width;	//get flat index from x,y coords
		
		tile_ids[flat_index] = type;
	}

	/*
		set a elevation type at position x,y : zero indexed
	*/
	void set(int x, int y, TileType type) {
		assert(initialised == true);
		assert(x > -1 && x < width);
		assert(y > -1 && y < height);

		int flat_index = x + y * width;	//get flat index from x,y coords

		tile_data[flat_index].type = type;
		tile_ids[flat_index] = type;
	}

	/*
		get a value at position x,y : zero indexed
	*/
	Tile get(int x, int y) {
		assert(initialised == true);
		assert(x > -1 && x < width);
		assert(y > -1 && y < height);

		int flat_index = x + y * width;
		return tile_data[flat_index];
	}

	uint8_t* getIDArray() {
		assert(initialised == true);
		assert(tile_data.size() == tile_ids.size());
		return &tile_ids[0];
	}
};


class Map
{
private:
	//metdata constants;
	float MNT_HT = 0.9f;
	const float DES_HT = 0.0f;
	float PLAIN_HT = 0.5f;
	const float OCEAN_HT = 0.0f;
	int MAX_RAND = 20;
	float smoothing_tresh = 0.4f;
	
	//map data
	const int map_width = 513;
	const int map_height = 513;
	TileMap tiles;

	//graphics data
	MapShader shader;
	GLuint vbo_id = -1;
	GLuint vao_id = -1;

	GLuint tiletex_id = -1;
	ILuint tileset_img = -1;

	bool draw_elevation = true;

	float randHeight(int scale);

	/*
		Generate vbo for tile map
	*/
	bool genVBO();

	/*
		update vbo with tilemap info
	*/
	bool updateVBO();

	/*
		Generate vao for tile map
	*/
	bool genVAO();

	/*
		Load tile textures
	*/
	bool loadTextures();

	/*
		Generate a random map
	*/
	void tileInit();

public:
	int getTransformLoc();
	bool initialise();
	void draw();
};

