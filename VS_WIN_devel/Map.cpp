#include "Map.h"

bool Map::initialise() {
	tile_data.reserve(map_width * map_height);
	tile_ids.reserve(map_width * map_height);

	//init tiles
	for (int y = 0; y < map_height; y++) {
		for (int x = 0; x < map_width; x++) {
			tile_data.push_back(Tile{ x,y });
			tile_ids.push_back(rand() % 4);
		}
	}

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

	genVBO();
	genVAO();

	return true;
}
bool Map::genVBO() {
	assert(tile_ids.size() != 0 && tile_data.size() == tile_ids.size());	//assert ids = number of tiles

	glGenBuffers(1, &vbo_id);	//generate a buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbo_id);	//bind the buffer to the global array buffer
	glBufferData(GL_ARRAY_BUFFER, tile_data.size() * sizeof(uint8_t), &tile_ids[0], GL_STATIC_DRAW);	//copy tile ids to the buffer
	
	if (vbo_id == -1) {
		printf("vbo gen error\n");
		return false;
	}

	return true;
}

bool Map::genVAO() {
	glGenVertexArrays(1, &vao_id);	//gen a vertex array object
	glBindVertexArray(vao_id);	//bind 
	glBindBuffer(GL_ARRAY_BUFFER, vbo_id);	//assign the generated vbo to global array buffer

	if (vao_id == -1) {
		printf("vao gen error\n");
		return false;
	}

	glEnableVertexAttribArray(0);	//enable tile id attribute in vertex shader 
	glVertexAttribIPointer(0, 1, GL_UNSIGNED_BYTE, sizeof(uint8_t), 0);
		//attrib 0
		//1 component
		//gl ubyte array
		//uint8_t stride (8 bytes)
		//0 offset

	return true;
}

bool Map::loadTextures() {
	ilInit();
	ilGenImages(1, &tileset_img);
	ilBindImage(tileset_img);
	ilLoadImage("D:\\Software and Tools\\C++\\T4x\\VS_WIN_devel\\resources\\TileSet.png");
	assert(ilGetInteger(IL_IMAGE_CHANNELS) == 3);	//assert rgb format

	ILubyte* bytes = ilGetData();	//get pixel data
	glGenTextures(1, &tiletex_id);
	glBindTexture(GL_TEXTURE_2D, tiletex_id);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, GL_RGB, GL_UNSIGNED_BYTE, bytes);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	if (glGetError() != GL_NO_ERROR) {
		printf("gl error: %d", glGetError());
	}
	return true;
}

void debug_matrix(glm::mat4& mat) {
	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			printf("%f ", mat[x][y]);
		}
		printf("\n");
	}
}

void Map::draw() {	
	glUseProgram(shader.getProgramID());
	glBindTexture(GL_TEXTURE_2D, tiletex_id);
	glBindVertexArray(vao_id);

	glm::mat4 proj = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	glm::mat4 scale = glm::scale(proj, glm::vec3(32.0f, 32.0f, 1.0f));
	glm::mat4 scale2 = glm::scale(proj, glm::vec3(2.0f/1280, 2.0f/720, 1.0f));
	proj = scale * scale2;
	//debug_matrix(proj);
	
	uint8_t a= 4;
	uint8_t b= 255;
	uint8_t c = a & b;


	//set mapsize uniform
	glUniform2i(glGetUniformLocation(shader.getProgramID(), "mapSize"), map_width, map_height);
	glUniformMatrix4fv(glGetUniformLocation(shader.getProgramID(), "projection"), 1, false, &proj[0][0]);

	//draw
	glUseProgram(shader.getProgramID());
	glDrawArrays(GL_POINTS, 0, map_width * map_height);
}
