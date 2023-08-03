#pragma once
//internal libraries
#include "ShaderManager.h"
#include "HeightMap.h"
#include "TempMap.h"
#include "TileMap.h"
#include "ImguiUtils.h"

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
#include <chrono>

//custom shaders for map
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

class Map
{
private:
	//simulation speed parameters
	int counter = 5;
	int speed = 10;

	float remap_time = 0.0f;
	float weather_update_time = 0.0f;
	float sim_times[7] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

	//tile map parameters
	TileMap tiles;
	float ocean_level = 0.5f;
	float beach_height = 0.02f;
	float mountain_height = 0.9f;
	float iron_ab = 0.0f;
	float chrom_ab = 0.0f;
	float copper_ab = 0.0f;
	float oil_ab = 0.0f;
	float coal_ab = 0.0f;

	//height map generation parameters
	HeightMap h_map;
	float perlin_freq = 0.2;	//perlin frequency
	float octave_weight = 1.0f;	//octave weight
	float fractal_gain = 0.2;	//fractal gain
	bool fractal_ridge = false;	//fractal toggle

	//temperature map parameters
	TempMap t_map;
	int sunx = -250;
	int equator = 0;
	float solar_intensity = 0.5;
	float radiation_factor = 0.1;
	int drop_off = 1;
	bool compute_temp = true;

	//map data
	const int map_width = 100;
	const int map_height = 100;

	//graphics data booleans
	bool draw_textures = true;
	bool draw_elevation = false;

	bool draw_pv = false;
	bool draw_air_temp = false;
	bool draw_surface_temp = false;
	bool draw_clouds = false;
	bool draw_pressure = false;
	bool draw_resources = false;

	//graphics handles
	MapShader shader;

	GLuint base_texture_id = -1;	//base texture handle
	GLuint base_vbo_id = -1;	//base texture vertex buffer object handle
	GLuint base_vao_id = -1;	//base texture array object handle

	GLuint overlay_texture_id = -1;	//overlay texture handle
	GLuint overlay_vbo_id = -1;	//overlay vertex buffer object handle
	GLuint overlay_vao_id = -1;	//overlay vertex array object handle

	/*
		Game methods
	*/
	//automatically generate a game map
	void autoGenerate();

	/*
		graphical methods
	*/
	//create a vbo buffer
	bool genVBO(GLuint* vbo_id);

	//copy data into a vbo
	bool updateVBO(GLuint vbo_id, int size, uint16_t* data);

	//create a vao and assign a vbo
	bool genVAO(GLuint* vao_id, GLuint vbo_id);

	//load texture file into a texture handle
	bool loadTextures(std::string texture_path, GLuint &texture_handle);

	//update base buffer
	void updateBase();
	//update overlay buffer
	void updateOverlay();

public:
	//loop map logic once
	void loop();
	
	//get tranform uniform location
	int getTransformLoc();

	//initialise game map
	bool initialise();

	//draw texture elements only
	void draw();

	/*
		imgui drawing functions
	*/
	//draw debug menu
	void drawDebug();

	//draw overhead display menu
	void drawDisplay();

	//draw simulation runtime values
	void drawSimulationValues();

	//draw map creation menu
	void drawCreationMenu();

};

