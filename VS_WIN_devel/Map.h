#pragma once
//internal libraries
#include "ShaderManager.h"
#include "HeightMap.h"
#include "TempMap.h"

//graphics libraries
#include "imgui.h"
#include <IL/il.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

//cstd libraries
#include <assert.h>
#include <cstdlib>
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

enum TileType : uint8_t { OCEAN, BEACH, PLAIN, FOREST, BLANK1, BLANK2, MOUNTAIN, PEAK};
enum ElevationType : uint8_t {L1, L2, L3, L4, L5, L6, L7, L8};

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

	int getSize() {
		return tile_data.size();
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

	//map generation constants;
	float perlin_freq = 0.2;	//perlin frequency
	float octave_weight = 1.0f;	//octave weight
	float fractal_gain = 0.2;	//fractal gain
	bool fractal_ridge = false;	//fractal toggle

	//temp constants;
	TempMap t_map;
	int sunx = 0;
	int suny = 0;
	bool compute_temp = true;

	//tile assignment values
	float land_range = 1.0f;
	float ocean_level = 0.5f;
	float beach_height = 0.2f;

	float plain_percentile = 0.1f;
	float forest_percentile = 0.3f;
	float mountain_percentile = 0.9f;

	//map data
	const int map_width = 513;
	const int map_height = 513;
	TileMap tiles;
	HeightMap h_map;

	//graphics data
	bool draw_elevation = false;
	bool draw_temp = false;

	MapShader shader;

	GLuint tiletex_id = -1;
	GLuint tile_vbo_id = -1;
	GLuint tile_vao_id = -1;

	GLuint overlay_id = -1;
	GLuint temp_vbo_id = -1;
	GLuint temp_vao_id = -1;

	/*
		map gen methods
	*/
	//assign textures based on current heigthmap
	void assignTiles();

	//create a vbo
	bool genVBO(GLuint* vbo_id);

	//copy data into a vbo
	bool updateVBO(GLuint vbo_id, int size, uint8_t* data);

	//create a vao and assign a vbo
	bool genVAO(GLuint* vao_id, GLuint vbo_id);

	/*
		Load tile textures
	*/
	bool loadTextures();

public:
	//loop map logic once
	void loop();
	int getTransformLoc();
	bool initialise();
	void draw();
};

