#pragma once
#include "ShaderManager.h"

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

enum TileType : uint8_t { PLAIN, MOUNTAIN, DESERT };

class Tile {
	int posx;
	int posy;

public:
	Tile(int x, int y) : posx{ x }, posy{ y } {
	}

	void draw() {

	}
};


class Map
{
private:
	//map data
	const int map_width = 10;
	const int map_height = 10;

	std::vector<Tile> tile_data;
	std::vector<uint8_t> tile_ids;

	//graphics data
	MapShader shader;
	GLuint vbo_id = -1;
	GLuint vao_id = -1;

	GLuint tiletex_id = -1;
	ILuint tileset_img = -1;

	/*
		Generate vbo for tile map
	*/
	bool genVBO();

	/*
		Generate vao for tile map
	*/
	bool genVAO();

	/*
		Load tile textures
	*/
	bool loadTextures();

public:
	bool initialise();
	void draw();
};

