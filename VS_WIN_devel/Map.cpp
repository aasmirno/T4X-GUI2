#include "Map.h"

int Map::getTransformLoc() {
	return glGetUniformLocation(shader.getProgramID(), "projection");
}

bool Map::initialise() {
	tiles.initialise(map_width, map_height);
	h_map.init(map_width, map_height);
	t_map.init(map_width, map_height);
	sunx = 0;
	suny = map_height / 2;

	assignTiles();

	/*
		graphical init
	*/
	loadTextures();

	glEnable(GL_BLEND);
	glClipControl(GL_UPPER_LEFT, GL_NEGATIVE_ONE_TO_ONE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (!shader.init()) {
		printf("map shader load issue\n");
		return false;
	}

	//generate base tile buffers
	genVBO(&tile_vbo_id);
	updateVBO(tile_vbo_id, map_height * map_width * sizeof(uint8_t), tiles.getIDArray()); //load tile data
	genVAO(&tile_vao_id, tile_vbo_id);

	//generate overlay buffers
	genVBO(&temp_vbo_id);
	updateVBO(temp_vbo_id, map_height * map_width * sizeof(uint8_t), t_map.getIDArray());	//load temp data
	genVAO(&temp_vao_id, temp_vbo_id);
	return true;
}

void Map::assignTiles() {
	h_map.normaliseHeightMap();
	for (int y = 0; y < map_width; y++) {
		for (int x = 0; x < map_width; x++) {
			float normalised_height = h_map.getHeight(x, y);

			if (h_map.getMaxHeight() == -1.0f) {
				normalised_height = h_map.getHeight(x, y) / h_map.getMaxHeight();
			}

			if (draw_elevation) {	//if elevation is to be drawn use ids in range {0,8}
				if (normalised_height < (1.0f / 7.0f)) {
					tiles.set(x, y, ElevationType::L1);
				}
				else if (normalised_height < (2.0f / 7.0f)) {
					tiles.set(x, y, ElevationType::L2);
				}
				else if (normalised_height < (3.0f / 7.0f)) {
					tiles.set(x, y, ElevationType::L3);
				}
				else if (normalised_height < (4.0f / 7.0f)) {
					tiles.set(x, y, ElevationType::L4);
				}
				else if (normalised_height < (5.0f / 7.0f)) {
					tiles.set(x, y, ElevationType::L5);
				}
				else if (normalised_height < (6.0f / 7.0f)) {
					tiles.set(x, y, ElevationType::L6);
				}
				else if (normalised_height < (6.5f / 7.0f)) {
					tiles.set(x, y, ElevationType::L7);
				}
				else {
					tiles.set(x, y, ElevationType::L8);
				}
			}
			else {	//use textures tile set in range {0,8}

				if (normalised_height < ocean_level) {
					tiles.set(x, y, TileType::OCEAN);
				}
				else if (normalised_height < ocean_level + beach_height) {
					tiles.set(x, y, TileType::BEACH);
				}
				else {
					tiles.set(x, y, TileType::PLAIN);
				}
			}
		}
	}
}

/*
	Loop section
*/
void Map::loop(){
	suny++;
	if (suny >= map_width) {
		suny = 0;
	}
	//do temp calculations
	if (compute_temp)
		t_map.update(sunx, suny);
	updateVBO(temp_vbo_id, map_height * map_width * sizeof(uint8_t), t_map.getIDArray());
}

/*
* Graphics Section
------------------------------------------------------------------------------
*/

bool Map::updateVBO(GLuint vbo_id, int size, uint8_t* array) {
	glBindBuffer(GL_ARRAY_BUFFER, vbo_id);	//bind the buffer to the global array buffer
	glBufferData(GL_ARRAY_BUFFER, size, array, GL_STATIC_DRAW);	//copy tile ids to the buffer
	return true;
}

bool Map::genVBO(GLuint *vbo_id) {
	glGenBuffers(1, vbo_id);	//generate a buffer
	if (*vbo_id == -1) {
		printf("vbo gen error\n");
		return false;
	}

	return true;
}

bool Map::genVAO(GLuint* vao_id, GLuint vbo_id) {
	glGenVertexArrays(1, vao_id);	//gen a vertex array object
	glBindVertexArray(*vao_id);		//bind vertex array object 
	
	assert(vbo_id != -1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_id);	//load the vbo to global context

	if (*vao_id == -1) {
		printf("vao gen error\n");
		return false;
	}

	glEnableVertexAttribArray(0);	//enable tile id attribute in vertex shader 
	glVertexAttribIPointer(0, 1, GL_UNSIGNED_BYTE, sizeof(uint8_t), 0);	//assign vbo to vao
	//attrib 0
	//1 component
	//gl ubyte array
	//uint8_t stride (8 bytes)
	//0 offset

	return true;
}

bool Map::loadTextures() {
	ILuint texture_img = -1;
	ilInit();
	ilGenImages(1, &texture_img);
	ilBindImage(texture_img);
	
	//load a base tile texture
	if (draw_elevation) {
		ilLoadImage("D:\\Software and Tools\\C++\\T4x\\VS_WIN_devel\\resources\\TileSetElevation32.png");
	}
	else {
		ilLoadImage("D:\\Software and Tools\\C++\\T4x\\VS_WIN_devel\\resources\\TileSet32.png");
	}
	assert(ilGetInteger(IL_IMAGE_CHANNELS) == 4);	//assert rgba format

	ILubyte* bytes = ilGetData();	//get pixel data
	glGenTextures(1, &tiletex_id);
	glBindTexture(GL_TEXTURE_2D, tiletex_id);

	//load the base tile texture into the opengl sample
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	if (glGetError() != GL_NO_ERROR) {
		printf("gl error: %d", glGetError());
	}

	//load an overlay texture
	ilLoadImage("D:\\Software and Tools\\C++\\T4x\\VS_WIN_devel\\resources\\OverlayTemp32.png");
	assert(ilGetInteger(IL_IMAGE_CHANNELS) == 4);	//assert rgba format
	bytes = ilGetData();	//get pixel data
	glGenTextures(1, &overlay_id);
	glBindTexture(GL_TEXTURE_2D, overlay_id);

	//load the overaly texture into the opengl sample
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	if (glGetError() != GL_NO_ERROR) {
		printf("gl error: %d", glGetError());
	}

	return true;
}

void Map::draw() {
	ImGui::SetNextWindowSize(ImVec2{ 450,600 });
	ImGui::Begin("internal map debug", NULL, ImGuiWindowFlags_NoResize);	//begin imgui window

	if (ImGui::Button("refresh map")) {
		loadTextures();
		h_map.refresh();
		t_map.refresh();
		assignTiles();
		updateVBO(tile_vbo_id, tiles.getSize(), tiles.getIDArray());
		updateVBO(temp_vbo_id, tiles.getSize(), t_map.getIDArray());
	}
	ImGui::SameLine();
	ImGui::Text("Sun pos: %d,%d", sunx, suny);

	ImGui::SeparatorText("generation algo testing");
	ImGui::Text("mapsize: %d,%d", map_width, map_height);
	ImGui::Text("max height: %f", h_map.getMaxHeight());
	ImGui::SliderFloat("perlin freq", &perlin_freq, 0.0001f, 0.05f);
	ImGui::SliderFloat("fractal gain", &fractal_gain, 0.2f, 0.8f);
	ImGui::SliderFloat("octave weight", &octave_weight, 0.0f, 1.0f);
	if (ImGui::Button("Toggle fractal ridged")) {
		fractal_ridge = !fractal_ridge;
	}
	ImGui::SameLine();
	ImGui::Text("%d", fractal_ridge);
	if (ImGui::Button("add perlin")) {
		h_map.addPerlin(perlin_freq, fractal_ridge, octave_weight);
		assignTiles();
		updateVBO(tile_vbo_id, tiles.getSize(), tiles.getIDArray());
	}
	ImGui::SameLine();
	if (ImGui::Button("sub perlin")) {
		h_map.subPerlin(perlin_freq, fractal_ridge, octave_weight);
		assignTiles();
		updateVBO(tile_vbo_id, tiles.getSize(), tiles.getIDArray());
	}

	if (ImGui::Button("erode hydro")) {
		h_map.erode();
		assignTiles();
		updateVBO(tile_vbo_id, tiles.getSize(), tiles.getIDArray());
	}

	ImGui::SeparatorText("tile type thresholds");
	ImGui::SliderFloat("ocean level", &ocean_level, 0.0f, 0.99f);
	ImGui::SliderFloat("beach height", &beach_height, 0.0f, 0.99f);
	ImGui::SliderFloat("mountain min level", &mountain_percentile, 0.0f, 1.0f);

	ImGui::SeparatorText("display options");
	if (ImGui::Button("Toggle Temp")) {
		draw_temp = !draw_temp;
		loadTextures();
		assignTiles();
		updateVBO(tile_vbo_id, tiles.getSize(), tiles.getIDArray());
	}
	ImGui::SameLine();
	if (ImGui::Button("Toggle Elevation")) {
		draw_elevation = !draw_elevation;
		loadTextures();
		assignTiles();
		updateVBO(tile_vbo_id, tiles.getSize(), tiles.getIDArray());
	}

	ImGui::End();//---------------------------------------------------------end imgui window

	/*
		opengl rendering
	*/
	//set mapsize uniform
	glUniform2i(glGetUniformLocation(shader.getProgramID(), "mapSize"), map_width, map_height);
	glUseProgram(shader.getProgramID());

	//bind and draw base
	glBindTexture(GL_TEXTURE_2D, tiletex_id);
	glBindVertexArray(tile_vao_id);
	glDrawArrays(GL_POINTS, 0, map_width * map_height);

	//bind and draw overlay
	if (draw_temp) {
		glBindTexture(GL_TEXTURE_2D, overlay_id);
		glBindVertexArray(temp_vao_id);
		glDrawArrays(GL_POINTS, 0, map_width * map_height);
	}
}
