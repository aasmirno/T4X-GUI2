#pragma once
//internal libraries
#include "ImguiUtils.h"
#include "ShaderManager.h"

#include "BaseMap.h"
#include "HeightMap.h"
#include "TempMap.h"
#include "TileMap.h"
#include "WaterMap.h"

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
#include <fstream>
#include <string>

//custom shaders for map
class MapShader :
	public ShaderManager
{
private:
	const GLint in_tile = 0;
public:
	bool init() {
		program_id = glCreateProgram();
		if (loadShader("D:\\Software and Tools\\C++\\T4x\\VS_WIN_devel\\resources\\mapvert.glvs", GL_VERTEX_SHADER, program_id) != true) {
			return false;
		}
		if (loadShader("D:\\Software and Tools\\C++\\T4x\\VS_WIN_devel\\resources\\mapfrag.glfs", GL_FRAGMENT_SHADER, program_id) != true) {
			return false;
		}
		if (loadShader("D:\\Software and Tools\\C++\\T4x\\VS_WIN_devel\\resources\\mapgeom.glgs", GL_GEOMETRY_SHADER, program_id) != true) {
			return false;
		}

		if (!linkProgram(program_id)) {
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
	//general parameters
	FastNoiseLite noise;

	//simulation speed parameters
	int counter = 5;
	int speed = 10;

	float remap_time = 0.0f;
	float weather_update_time = 0.0f;
	float sim_times[7] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

	//tile map parameters
	TileMap tiles;
	float ocean_level = 0.42f;
	float beach_height = 0.02f;
	float mountain_height = 0.9f;

	uint8_t resource_method = 0;
	float iron_ab = 0.0001f;	int iron_sz = 6;
	float chrom_ab = 0.0f;	int chrm_sz = 3;
	float copper_ab = 0.0f; int cu_sz = 6;
	float oil_ab = 0.0f;	int oil_sz = 5;
	float coal_ab = 0.0002f;	int coal_sz = 10;

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

	//water map params
	WaterMap water;
	float args[11] = {
			1.0f,	//time increment
			1.0f,	//cross sectional area of connecting pipes
			9.8f,	//gravity constant
			0.5f,	//pipe length
			0.1f,	//grid x distance
			0.1f,	//grid y distance
			0.5f,	//sediment capacity constant
			0.3f,	//dissolving constant
			1.0f,	//deposition constant
			0.2f,	//minimum tilt angle
			0.2f	//evap const
	};

	//map data
	int map_width = 700;
	int map_height = 700;

	//graphics data booleans
	bool draw_textures = true;
	bool draw_elevation = false;

	bool draw_pv = false;
	bool draw_air_temp = false;
	bool draw_surface_temp = false;
	bool draw_clouds = false;
	bool draw_pressure = false;
	bool draw_resources = false;
	bool draw_water = false;
	bool draw_velocity = false;

	//graphics meta
	bool* c_flag = nullptr; //continue flag
	char last_file_name[1024 * 16];
	bool load_error = false;
	std::string r_method;

	//graphics handles
	MapShader shader;

	//base textures
	MapShader::GLTexture TEX_TILE32;
	MapShader::GLTexture TEX_ELEVATION32;
	//overlays
	MapShader::GLTexture TEX_CLOUDS32_O;
	MapShader::GLTexture TEX_TEMP32_O;
	MapShader::GLTexture TEX_RESOURCES32_O;
	MapShader::GLTexture TEX_WATER32_O;
	MapShader::GLTexture TEX_VELO_O;

	//vaos and vbos
	GLuint base_vbo_id = -1;	//base texture vertex buffer object handle
	GLuint base_vao_id = -1;	//base texture array object handle

	GLuint overlay_vbo_id = -1;	//overlay vertex buffer object handle
	GLuint overlay_vao_id = -1;	//overlay vertex array object handle

private:
	/*
		Game methods
	*/
	//automatically generate a game map
	void autoGenerate();
	void autoGeneratePlate();

	/*
		graphical methods
	*/
	//update base buffer
	void updateBase();

	//update overlay buffer
	void updateOverlay();

	/*
		Utils
	*/

	bool saveHeightMap(const std::string& file_name);
	bool loadHeightMap(const std::string& file_name);
public:
	//loop map logic once
	void loop();

	//get tranform uniform location
	int getTransformLoc();

	//initialise game map
	bool initialise(bool* cont_flag);

	//draw texture elements only
	void draw(int mx, int my);

	/*
		imgui drawing functions
	*/
	//draw debug menu
	void drawDebug(int mx, int my);

	//draw overhead display menu
	void drawDisplay();

	//draw simulation runtime values
	void drawSimulationValues();

	//draw map creation menu
	void drawCreationMenu();

};

