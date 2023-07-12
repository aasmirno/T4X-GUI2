#include "Map.h"

int Map::getTransformLoc() {
	return glGetUniformLocation(shader.getProgramID(), "projection");
}

bool Map::initialise() {
	tiles.initialise(map_width, map_height);
	tileRefresh();

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

/*
* Map generation section
------------------------------------------------------------------------------
*/
int Map::randHeight(int scale) {
	if (scale <= 0) {
		scale = 1;
	}
	std::random_device rd;
	std::mt19937 generator(rd());

	std::uniform_int_distribution<std::mt19937::result_type> distribution(0, 0 + scale);
	return distribution(generator);
}

//refresh the height map
void Map::tileRefresh() {
	height_map.clear();
	height_map.resize(map_width);	//resize the height map for memory saving
	for (auto i = 0; i < height_map.size(); i++) {
		height_map[i].resize(map_width);
	}
	assignTiles();
}

void Map::addPerlin() {
	FastNoiseLite noise;
	noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

	if (fractal_ridge) {
		noise.SetFractalType(FastNoiseLite::FractalType_Ridged);
	}

	//generate 1st octave perlin noise
	std::random_device rd;
	noise.SetSeed(rd());
	noise.SetFrequency(perlin_freq);
	for (int y = 0; y < map_height; y++)
	{
		for (int x = 0; x < map_width; x++)
		{
			height_map[y][x] += ((noise.GetNoise((float)x, (float)y) + 1.0) * octave_weight);
		}
	}
	//normaliseHeightMap();
}

void Map::subPerlin() {
	FastNoiseLite noise;
	noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

	if (fractal_ridge) {
		noise.SetFractalType(FastNoiseLite::FractalType_Ridged);
	}

	//generate 1st octave perlin noise
	std::random_device rd;
	noise.SetSeed(rd());
	noise.SetFrequency(perlin_freq);
	for (int y = 0; y < map_height; y++)
	{
		for (int x = 0; x < map_width; x++)
		{
			height_map[y][x] -= ((noise.GetNoise((float)x, (float)y) + 1.0) * octave_weight);
		}
	}
	//normaliseHeightMap();
}

void Map::normaliseHeightMap() {
	float max_height = 0.0f;
	//normalise heights
	for (int y = 0; y < map_width; y++) {
		for (int x = 0; x < map_width; x++) {
			if (height_map[y][x] > max_height) {
				max_height = height_map[y][x];
			}
		}
	}

	//normalise heights
	for (int y = 0; y < map_width; y++) {
		for (int x = 0; x < map_width; x++) {
			height_map[y][x] = height_map[y][x] / (max_height + 1);
		}
	}
}

void Map::assignTiles() {
	for (int y = 0; y < map_width; y++) {
		for (int x = 0; x < map_width; x++) {
			float normalised_height = height_map[y][x];
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
				else if (normalised_height < (1.0f / 6.0f)) {
					tiles.set(x, y, TileType::BEACH);
				}
				else if (normalised_height < (2.0f / 6.0f)) {
					tiles.set(x, y, TileType::BEACH);
				}
				else if (normalised_height < (3.0f / 6.0f)) {
					tiles.set(x, y, TileType::FOREST);
				}
				else if (normalised_height < (4.0f / 6.0f)) {
					tiles.set(x, y, TileType::FOREST);
				}
				else if (normalised_height < (5.0f / 6.0f)) {
					tiles.set(x, y, TileType::MOUNTAIN);
				}
				else{
					tiles.set(x, y, TileType::PEAK);
				}
			}
		}
	}
}

bool Map::coordCheck(int x, int y) {
	if (x < 0 || x >= map_width) {
		return false;
	}
	if (y < 0 || y >= map_height) {
		return false;
	}
	return true;
}

void Map::erodeHydro() {
	int num_droplets = 1000;
	for (num_droplets; num_droplets > 0; num_droplets--) {
		int x, y;

		//choose a random point
		x = rand()%map_width;
		y = rand()%map_height;
		float erosion = 0.3;

		bool flowing = true;
		int num_tiles = 0;

		//generate droplet and run down hill
		while (flowing) {
			if (tiles.get(x,y).type == TileType::OCEAN) {
				height_map[y][x] += (erosion * num_tiles);
				flowing = false;
			}
			else {

				//find greatest gradient
				int nextx = x;
				int nexty = y;
				float lowest_height = height_map[y][x];

				if (coordCheck(x - 1, y - 1) && height_map[y - 1][x - 1] < lowest_height) {
					nextx = x - 1;
					nexty = y - 1;
					lowest_height = height_map[y - 1][x - 1];
				}
				if (coordCheck(x - 1, y) && height_map[y][x - 1] < lowest_height) {
					nextx = x - 1;
					nexty = y;
					lowest_height = height_map[y][x - 1];
				}
				if (coordCheck(x - 1, y + 1) && height_map[y + 1][x - 1] < lowest_height) {
					nextx = x - 1;
					nexty = y + 1;
					lowest_height = height_map[y + 1][x - 1];
				}

				if (coordCheck(x, y - 1) && height_map[y - 1][x] < lowest_height) {
					nextx = x;
					nexty = y - 1;
					lowest_height = height_map[y - 1][x];
				}
				if (coordCheck(x, y + 1) && height_map[y + 1][x] < lowest_height) {
					nextx = x;
					nexty = y + 1;
					lowest_height = height_map[y + 1][x];
				}

				if (coordCheck(x + 1, y - 1) && height_map[y - 1][x + 1] < lowest_height) {
					nextx = x + 1;
					nexty = y - 1;
					lowest_height = height_map[y - 1][x + 1];
				}
				if (coordCheck(x + 1, y) && height_map[y][x + 1] < lowest_height) {
					nextx = x + 1;
					nexty = y;
					lowest_height = height_map[y][x + 1];
				}
				if (coordCheck(x + 1, y + 1) && height_map[y + 1][x + 1] < lowest_height) {
					nextx = x + 1;
					nexty = y + 1;
					lowest_height = height_map[y + 1][x + 1];
				}

				//if no lowest point end flow
				if (x == nextx && y == nexty) {
					flowing = false;
				}
				else {
					//erode
					height_map[y][x] -= erosion;
					num_tiles++;
					x = nextx;
					y = nexty;
				}


			}
		}
	}
}

/*
* Graphics Section
------------------------------------------------------------------------------
*/

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

	ImGui::SetNextWindowSize(ImVec2{ 350,400 });
	ImGui::Begin("internal map debug", NULL, ImGuiWindowFlags_NoResize);	//begin imgui window

	ImGui::SeparatorText("generation algo testing");
	ImGui::Text("mapsize: %d,%d", map_width, map_height);
	ImGui::SliderFloat("perlin freq", &perlin_freq, 0.0001f, 0.05f);
	ImGui::SliderFloat("fractal gain", &fractal_gain, 0.2f, 0.8f);
	ImGui::SliderFloat("octave weight", &octave_weight, 0.0f, 1.0f);
	if (ImGui::Button("Toggle fractal ridged")) {
		fractal_ridge = !fractal_ridge;
	}
	ImGui::SameLine();
	ImGui::Text("%d", fractal_ridge);

	if (ImGui::Button("add perlin")) {
		addPerlin();
		assignTiles();
		updateVBO();
	}
	ImGui::SameLine();
	if (ImGui::Button("sub perlin")) {
		subPerlin();
		assignTiles();
		updateVBO();
	}

	if (ImGui::Button("erode hydro")) {
		erodeHydro();
		assignTiles();
		updateVBO();
	}

	ImGui::SeparatorText("tile type thresholds");
	ImGui::SliderFloat("ocean level", &ocean_level, 0.0f, 0.99f);
	ImGui::SliderFloat("mountain min level", &min_mountain, 0.0f, 1.0f);

	if (ImGui::Button("refresh map")) {
		loadTextures();
		tileRefresh();
		updateVBO();
	}

	ImGui::SameLine();
	if (ImGui::Button("Toggle Elevation on current map")) {
		draw_elevation = !draw_elevation;
		loadTextures();
		assignTiles();
		updateVBO();
	}

	ImGui::End();//---------------------------------------------------------end imgui window

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
