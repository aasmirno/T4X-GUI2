#include "Map.h"

int Map::getTransformLoc() {
	return glGetUniformLocation(shader.getProgramID(), "projection");
}


float Map::randHeight(int scale) {
	if (scale <= 0) {
		scale = 1;
	}
	std::random_device rd;
	std::mt19937 generator(rd());

	std::uniform_int_distribution<std::mt19937::result_type> distribution(0, 0 + scale);
	return distribution(generator);
}

/*
	generate a random map using diamond square algorithm
*/
void Map::tileInit() {
	int random_scale = MAX_RAND;
	float max_height = 0.0f;

	std::vector<std::vector<float>> height_map;
	height_map.resize(map_width);
	for (auto i = 0; i < height_map.size(); i++) {
		height_map[i].resize(map_width);
	}
	height_map[0][0] = (randHeight(random_scale)) / MAX_RAND;
	height_map[map_width - 1][0] = (randHeight(random_scale)) / MAX_RAND;
	height_map[0][map_width - 1] = (randHeight(random_scale)) / MAX_RAND;
	height_map[map_width - 1][map_width - 1] = (randHeight(random_scale)) / MAX_RAND;

	if (height_map[0][0] > max_height) {
		max_height = height_map[0][0];
	}
	if (height_map[map_width - 1][0] > max_height) {
		max_height = height_map[map_width - 1][0];
	}
	if (height_map[0][map_width - 1] > max_height) {
		max_height = height_map[0][map_width - 1];
	}
	if (height_map[map_width - 1][map_width - 1] > max_height) {
		max_height = height_map[map_width - 1][map_width - 1];
	}


	for (int iter = map_width - 1; iter > 1; iter /= 2) {
		random_scale /= 2;

		for (int y = 0; y < map_width - 1; y += iter) {
			for (int x = 0; x < map_width - 1; x += iter) {

				float avg = (height_map[y][x] + height_map[y + iter][x] + height_map[y][x + iter] + height_map[y + iter][x + iter]) / 4.0f;

				height_map[y + (iter / 2)][x + (iter / 2)] = avg + (randHeight(random_scale));

				if (height_map[y + (iter / 2)][x + (iter / 2)] > max_height) {
					max_height = height_map[y + (iter / 2)][x + (iter / 2)];
				}

			}
		}

		for (int y = 0; y < map_width - 1; y += iter) {
			for (int x = 0; x < map_width - 1; x += iter) {
				float s0 = height_map[y][x];
				float s1 = height_map[y][x + iter];
				float s2 = height_map[y + iter][x];
				float s3 = height_map[y + iter][x + iter];
				float cn = height_map[y + (iter / 2)][x + (iter / 2)];

				float d0 = y <= 0 ? (s0 + s1 + cn) / 3.0f : (s0 + s1 + cn + height_map[y - (iter / 2)][x + (iter / 2)]) / 4.0f;
				float d1 = x <= 0 ? (s0 + cn + s2) / 3.0f : (s0 + cn + s2 + height_map[y + (iter / 2)][x - (iter / 2)]) / 4.0f;
				float d2 = x >= map_height - 1 - iter ? (s1 + cn + s3) / 3.0f :
					(s1 + cn + s3 + height_map[y + (iter / 2)][x + iter + (iter / 2)]) / 4.0f;
				float d3 = y >= map_height - 1 - iter ? (cn + s2 + s3) / 3.0f :
					(cn + s2 + s3 + height_map[y + iter + (iter / 2)][x + (iter / 2)]) / 4.0f;

				height_map[y][x + (iter / 2)] = d0 + (randHeight(random_scale)) / MAX_RAND;
				height_map[y + (iter / 2)][x] = d1 + (randHeight(random_scale)) / MAX_RAND;
				height_map[y + (iter / 2)][x + iter] = d2 + (randHeight(random_scale)) / MAX_RAND;
				height_map[y + iter][x + (iter / 2)] = d3 + (randHeight(random_scale)) / MAX_RAND;

				if (height_map[y][x + (iter / 2)] > max_height) {
					max_height = height_map[y][x + (iter / 2)];
				}
				if (height_map[y + (iter / 2)][x] > max_height) {
					max_height = height_map[y + (iter / 2)][x];
				}
				if (height_map[y + (iter / 2)][x + iter] > max_height) {
					max_height = height_map[y + (iter / 2)][x + iter];
				}
				if (height_map[y + iter][x + (iter / 2)] > max_height) {
					max_height = height_map[y + iter][x + (iter / 2)];
				}
			}
		}

	}


	for (int y = 0; y < map_width; y++) {
		for (int x = 0; x < map_width; x++) {
			height_map[y][x] = height_map[y][x] / max_height;

			if (draw_elevation) {
				if (height_map[y][x] < (1.0f/7.0f)) {
					tiles.set(x, y, ElevationType::L1);
				}
				else if (height_map[y][x] < (2.0f / 7.0f)) {
					tiles.set(x, y, ElevationType::L2);
				}
				else if (height_map[y][x] < (3.0f / 7.0f)) {
					tiles.set(x, y, ElevationType::L3);
				}
				else if (height_map[y][x] < (4.0f / 7.0f)) {
					tiles.set(x, y, ElevationType::L4);
				}
				else if (height_map[y][x] < (5.0f / 7.0f)) {
					tiles.set(x, y, ElevationType::L5);
				}
				else if (height_map[y][x] < (6.0f / 7.0f)) {
					tiles.set(x, y, ElevationType::L6);
				}
				else {
					tiles.set(x, y, ElevationType::L7);
				}
			}
			else {
				if ((height_map[y][x] - smoothing_tresh) < PLAIN_HT) {
					tiles.set(x, y, TileType::RIVER);
				}
				else if (height_map[y][x] < MNT_HT) {
					tiles.set(x, y, TileType::PLAIN);
				}
				else {
					tiles.set(x, y, TileType::MOUNTAIN);
				}
			}
		}
	}



}

bool Map::initialise() {
	tiles.initialise(map_width, map_height);
	tileInit();

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

bool Map::updateVBO() {
	glBindBuffer(GL_ARRAY_BUFFER, vbo_id);	//bind the buffer to the global array buffer
	glBufferData(GL_ARRAY_BUFFER, map_width * map_height * sizeof(uint8_t), tiles.getIDArray(), GL_STATIC_DRAW);	//copy tile ids to the buffer
	return true;
}

bool Map::genVBO() {

	glGenBuffers(1, &vbo_id);	//generate a buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbo_id);	//bind the buffer to the global array buffer
	glBufferData(GL_ARRAY_BUFFER, map_width * map_height * sizeof(uint8_t), tiles.getIDArray(), GL_STATIC_DRAW);	//copy tile ids to the buffer

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
	if (draw_elevation) {
		ilLoadImage("D:\\Software and Tools\\C++\\T4x\\VS_WIN_devel\\resources\\TileSetElevation32.png");
	}
	else {
		ilLoadImage("D:\\Software and Tools\\C++\\T4x\\VS_WIN_devel\\resources\\TileSet32.png");
	}
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

void Map::draw() {
	glBindTexture(GL_TEXTURE_2D, tiletex_id);
	glBindVertexArray(vao_id);

	ImGui::Begin("internal map debug");
	ImGui::SeparatorText("generation algo testing");
	ImGui::SliderInt("starting random range", &MAX_RAND, 0, 100);
	ImGui::SeparatorText("tile type thresholds");
	ImGui::SliderFloat("plain float", &PLAIN_HT, 0.0f, 1.0f, "%.4f");
	ImGui::SliderFloat("smoothing float", &smoothing_tresh, 0.0f, 1.0f, "%.4f");
	if (ImGui::Button("regen map")) {
		loadTextures();
		tileInit();
		updateVBO();
	}
	ImGui::SameLine();
	ImGui::Checkbox("draw elevation", &draw_elevation);
	ImGui::End();

	//glm::mat4 proj = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	//glm::mat4 scale = glm::scale(proj, glm::vec3(64.0f, 64.0f, 1.0f));
	//glm::mat4 scale2 = glm::scale(proj, glm::vec3(2.0f/1280, 2.0f/720, 1.0f));
	//proj = scale * scale2;
	////debug_matrix(proj);


	////set mapsize uniform
	glUniform2i(glGetUniformLocation(shader.getProgramID(), "mapSize"), map_width, map_height);
	//glUniformMatrix4fv(glGetUniformLocation(shader.getProgramID(), "projection"), 1, false, &proj[0][0]);

	//draw
	glUseProgram(shader.getProgramID());
	glDrawArrays(GL_POINTS, 0, map_width * map_height);
}
