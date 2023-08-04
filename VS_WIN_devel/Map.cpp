#include "Map.h"

int Map::getTransformLoc() {
	return glGetUniformLocation(shader.getProgramID(), "projection");
}

bool Map::initialise() {
	tiles.initialise(map_width, map_height);	//initialise tile map
	h_map.init(map_width, map_height);	//initialise height map
	t_map.init(map_width, map_height);	//initialise temperature map
	equator = map_height / 2;		//set sun y level
	
	/*
		graphical init
	*/
	loadTextures("D:\\Software and Tools\\C++\\T4x\\VS_WIN_devel\\resources\\TileSet32.png", base_texture_id);	//load base texture (default: textured tiles)
	loadTextures("D:\\Software and Tools\\C++\\T4x\\VS_WIN_devel\\resources\\OverlayTemp32.png", overlay_texture_id);	//load overlay texture (default: temperature)

	glEnable(GL_BLEND);
	glClipControl(GL_UPPER_LEFT, GL_NEGATIVE_ONE_TO_ONE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//initialise shader
	if (!shader.init()) {
		printf("map shader load issue\n");
		return false;
	}

	//generate base texture buffers
	genVBO(&base_vbo_id);	//generate base texture buffer
	updateVBO(base_vbo_id, map_height * map_width, tiles.getIDArray(h_map.getHeightMap(), ocean_level, beach_height, mountain_height,0)); //load tile texture ids into base buffer
	genVAO(&base_vao_id, base_vbo_id);	//generate base texture vao

	//generate overlay buffers
	genVBO(&overlay_vbo_id);	//generate overlay array buffer
	updateVBO(overlay_vbo_id, map_height * map_width, t_map.getIDArray(0));	//load air temperature ids into overlay buffer
	genVAO(&overlay_vao_id, overlay_vbo_id);	//generate overlay array buffer
	return true;
}

/*
* Gameplay and utility Section
------------------------------------------------------------------------------
*/
void Map::loop() {
	if (counter <= 0) {
		auto begin = std::chrono::high_resolution_clock::now();
		auto end = std::chrono::high_resolution_clock::now();
		remap_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

		sunx++;
		if (sunx >= map_width / 2 + map_width) {
			sunx = -map_width / 2;
		}

		begin = std::chrono::high_resolution_clock::now();
		//do temp calculations
		if (compute_temp) {
			float* vecy = t_map.update(sunx, equator, solar_intensity, radiation_factor, drop_off, h_map.getHeightMap(), ocean_level);
			sim_times[0] = vecy[0];
			sim_times[1] = vecy[1];
			sim_times[2] = vecy[2];
			sim_times[3] = vecy[3];
			sim_times[4] = vecy[4];
			sim_times[5] = vecy[5];
			sim_times[6] = vecy[6];
		}
		end = std::chrono::high_resolution_clock::now();
		weather_update_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

		//update textures
		if (draw_air_temp) {
			updateVBO(overlay_vbo_id, map_height * map_width * sizeof(uint16_t), t_map.getIDArray(0));
		}
		else if (draw_surface_temp) {
			updateVBO(overlay_vbo_id, map_height * map_width * sizeof(uint16_t), t_map.getIDArray(1));
		}
		else if (draw_clouds) {
			updateVBO(overlay_vbo_id, map_height * map_width * sizeof(uint16_t), t_map.getIDArray(2));
		}
		else if (draw_pressure) {
			updateVBO(overlay_vbo_id, map_height * map_width * sizeof(uint16_t), t_map.getIDArray(3));
		}

		counter = speed;
	}
	else
	{
		counter--;
	}
}

void Map::autoGenerate() {
	//refresh internal maps
	h_map.refresh();
	t_map.refresh();
	tiles.refresh();
	sunx = 0;

	//reset graphical flags
	draw_surface_temp, draw_air_temp = false;

	//first addition
	perlin_freq = 0.004;
	fractal_ridge = false;
	octave_weight = 0.4f;
	h_map.addNoise(perlin_freq, fractal_ridge, octave_weight);

	//second addition
	perlin_freq = 0.01f;
	fractal_ridge = false;	
	octave_weight = 0.1f;
	h_map.addNoise(perlin_freq, fractal_ridge, octave_weight);

	//detailing
	for (int i = 0; i < 10; i++) {
		h_map.addNoise(0.05f, true, 0.01f);
		h_map.subNoise(0.05f, false, 0.006f);
	}

}

/*
* Graphics Section
------------------------------------------------------------------------------
*/

bool Map::updateVBO(GLuint vbo_id, int size, uint16_t* array) {
	glBindBuffer(GL_ARRAY_BUFFER, vbo_id);	//bind the buffer to the global array buffer
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(uint16_t), array, GL_STATIC_DRAW);	//copy tile ids to the buffer

	GLenum err;
	if ((err = glGetError()) != GL_NO_ERROR) {
		std::cout << "gl error (Map::loadTextures): " << err << std::endl;
	}

	return true;
}

bool Map::genVBO(GLuint* vbo_id) {
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
	glVertexAttribIPointer(0, 1, GL_UNSIGNED_SHORT, 0, 0);	//assign vbo to vao
	//attrib 0
	//1 component
	//gl ushort(16) array
	//uint16_t stride (16 bytes)
	//0 offset

	return true;
}

bool Map::loadTextures(std::string texture_path, GLuint& texture_handle) {
	//create an openil image
	ILuint img = -1;
	ilInit();
	ilGenImages(1, &img);	//gen img buffer
	ilBindImage(img);	//bind created image

	if (texture_handle == -1) {	//check if texture is already generated
		glGenTextures(1, &texture_handle);	//generate a texture
	}
	glBindTexture(GL_TEXTURE_2D, texture_handle);	//bind the texture handle to opengl

	//load the texture file
	ilLoadImage(&texture_path[0]);	//load
	assert(ilGetInteger(IL_IMAGE_CHANNELS) == 4);	//assert rgba format
	ILubyte* bytes = ilGetData();	//get pixel data

	//load the base tile texture into the opengl sampler
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	GLenum err;
	if ((err = glGetError()) != GL_NO_ERROR) {
		std::cout << "gl error (Map::loadTextures): " << err << std::endl;
		std::cout << "	glu string: " << glewGetErrorString(err) << std::endl;
	}

	//delete openil image
	ilDeleteImages(1, &img);
	return true;
}

void Map::draw(int mx, int my) {
	/*
		opengl rendering
	*/
	glUniform2i(glGetUniformLocation(shader.getProgramID(), "mapSize"), map_width, map_height); //set mapsize uniform
	glUseProgram(shader.getProgramID());	//use shader program

	drawDebug(mx,my);

	//bind and draw base textures
	glBindTexture(GL_TEXTURE_2D, base_texture_id);
	glBindVertexArray(base_vao_id);
	glDrawArrays(GL_POINTS, 0, map_width * map_height);

	//bind and draw overlay textures
	if (draw_air_temp || draw_surface_temp || draw_clouds || draw_resources || draw_pv) {
		glBindTexture(GL_TEXTURE_2D, overlay_texture_id);
		glBindVertexArray(overlay_vao_id);
		glDrawArrays(GL_POINTS, 0, map_width * map_height);
	}
}

void Map::updateBase() {
	if (draw_elevation) {
		loadTextures("D:\\Software and Tools\\C++\\T4x\\VS_WIN_devel\\resources\\TileSetElevation32.png", base_texture_id);	//load tile textures
		updateVBO(base_vbo_id, t_map.getSize(), h_map.getIDArray());	//update elevation
	}
	else if (draw_textures) {
		loadTextures("D:\\Software and Tools\\C++\\T4x\\VS_WIN_devel\\resources\\TileSet32.png", base_texture_id);
		updateVBO(base_vbo_id, tiles.getSize(), tiles.getIDArray(h_map.getHeightMap(), ocean_level, beach_height, mountain_height, 0));	//update tile textures
	}
}

void Map::updateOverlay() {
	if (draw_surface_temp) {
		loadTextures("D:\\Software and Tools\\C++\\T4x\\VS_WIN_devel\\resources\\OverlayTemp32.png", overlay_texture_id);	//load temp tex
		updateVBO(overlay_vbo_id, t_map.getSize(), t_map.getIDArray(1));
	}
	else if (draw_clouds) {
		loadTextures("D:\\Software and Tools\\C++\\T4x\\VS_WIN_devel\\resources\\OverlayClouds32.png", overlay_texture_id);	//load cloud tex
		updateVBO(overlay_vbo_id, t_map.getSize(), t_map.getIDArray(2));
	}
	else if (draw_resources) {
		loadTextures("D:\\Software and Tools\\C++\\T4x\\VS_WIN_devel\\resources\\OverlayResource32.png", overlay_texture_id);	//load resource tex
		updateVBO(overlay_vbo_id, tiles.getSize(), tiles.getIDArray(h_map.getHeightMap(), ocean_level, beach_height, mountain_height, 1));
	}
	else if (draw_pv) {
		loadTextures("D:\\Software and Tools\\C++\\T4x\\VS_WIN_devel\\resources\\OverlayTemp32.png", overlay_texture_id);	//load temp tex
		updateVBO(overlay_vbo_id, tiles.getSize(), h_map.getPlateOverlay());
	}
}

/*
* imgui section
------------------------------------------------------------------------------
*/
void Map::drawDebug(int mx, int my) {
	ImGui::SetNextWindowSize(ImVec2{ 450,600 });
	ImGui::Begin("internal map debug", NULL, ImGuiWindowFlags_NoResize);	//begin imgui window
	if (ImGui::Button("refresh map")) {
		//refresh internal maps
		h_map.refresh();
		t_map.refresh();
		tiles.refresh();
		sunx = 0;

		//reset graphical flags
		draw_surface_temp, draw_air_temp = false;

		//load textured tiles into base handle
		loadTextures("D:\\Software and Tools\\C++\\T4x\\VS_WIN_devel\\resources\\TileSet32.png", base_texture_id);

		//update base vbo
		updateBase();
	}
	ImGui::SameLine();
	if (ImGui::Button("refresh temp/weather_update_time")) {
		t_map.refresh();
		sunx = 0;
	}

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
		h_map.addNoise(perlin_freq, fractal_ridge, octave_weight);	//apply perlin noise
		updateBase();
	}
	ImGui::SameLine();
	if (ImGui::Button("sub perlin")) {
		h_map.subNoise(perlin_freq, fractal_ridge, octave_weight);
		updateBase();
	}

	if (ImGui::Button("erode hydro")) {
		h_map.erode();
	}

	ImGui::SeparatorText("tile type thresholds");
	ImGui::SliderFloat("ocean level", &ocean_level, 0.0f, 1.0f);
	ImGui::SliderFloat("beach height", &beach_height, 0.0f, 1.0f);
	ImGui::SliderFloat("mountain min level", &mountain_height, 0.0f, 1.0f);

	ImGui::SeparatorText("temp map factors");
	ImGui::SliderFloat("solar intensity", &solar_intensity, 0.0f, 50.0f);
	ImGui::SliderInt("equator", &equator, 0, map_height);
	ImGui::SliderFloat("radiation factor", &radiation_factor, 0.5f, 0.6f);
	ImGui::SliderInt("intensity gradient", &drop_off, 0, 500);

	ImGui::Text("added: %f, lost %f", t_map.getAdded(), t_map.getRadiated());
	if (ImGui::Button("tectonic generation")) {
		h_map.initialisePlates();
		updateBase();
	}

	if (ImGui::Button("draw pv")) {
		draw_air_temp = false, draw_surface_temp = false, draw_clouds = false, draw_resources = false;
		draw_pv = true;
		updateOverlay();
	}

	float height_at_mouse = 0.0f;
	if (mx > 0 && mx < map_width && my > 0 && my < map_height) {
		height_at_mouse = h_map.getHeight(mx, my);
	}
	ImGui::Text("height at (%d,%d): %f", mx, my, height_at_mouse);

	ImGui::End();
}

void Map::drawDisplay() {
	ImVec2 button_size = ImVec2(130, 20);

	ImGuiWindowFlags flags = 0;
	flags |= ImGuiWindowFlags_NoMove;
	flags |= ImGuiWindowFlags_NoResize;
	flags |= ImGuiWindowFlags_NoTitleBar;

	ImGui::SetNextWindowPos(ImVec2{ 340,10 });
	ImGui::SetNextWindowSize(ImVec2(540, 66));
	ImGui::Begin("Display", NULL, flags);

	ImGui::SameLine(0, 4);
	if (ImGui::Button("-", ImVec2(20, 20)))
	{
		speed--;
		speed = speed < 0 ? 0 : speed;
	}
	ImGui::SameLine();
	ImGui::Text("simulation speed: %d", speed);
	ImGui::SameLine();
	if (ImGui::Button("+", ImVec2(20, 20)))
	{
		speed++;
		speed = speed > 100 ? 100 : speed;
	}
	ImGui::SameLine();
	ImGui::Text("| Sun pos: %d,%d", sunx, equator);

	ImGui::Text("");
	ImGui::SameLine(0, 4);
	if (ImGui::Button("Draw tiles", button_size)) {
		draw_textures = true;
		draw_elevation = false;
		updateBase();
	}
	ImGui::SameLine();
	if (ImGui::Button("Draw elevation", button_size)) {
		draw_textures = false;
		draw_elevation = true;
		updateBase();
	}

	ImGui::Text("");
	ImGui::SameLine(0, 4);
	if (ImGui::Button("No overlay", button_size)) {
		draw_air_temp = false, draw_surface_temp = false, draw_clouds = false, draw_resources = false, draw_pv = false;
	}
	ImGui::SameLine();
	if (ImGui::Button("Draw surface temp", button_size)) {
		draw_air_temp = false, draw_clouds = false, draw_pressure = false, draw_pv = false;
		draw_surface_temp = true;
		updateOverlay();
	}
	ImGui::SameLine();
	if (ImGui::Button("Draw cloud cover", button_size)) {
		draw_air_temp = false, draw_surface_temp = false, draw_pressure = false, draw_pv = false;
		draw_clouds = true;

		updateOverlay();
	}
	ImGui::SameLine();
	if (ImGui::Button("Draw resources", button_size)) {
		draw_air_temp = false, draw_surface_temp = false, draw_pressure = false, draw_clouds = false, draw_pv = false;
		draw_resources = true;

		updateOverlay();
	}
	ImGui::End();
}

void Map::drawSimulationValues() {
	ImGuiWindowFlags flags = 0;
	flags |= ImGuiWindowFlags_NoMove;
	flags |= ImGuiWindowFlags_NoResize;
	flags |= ImGuiWindowFlags_NoTitleBar;

	ImGui::SetNextWindowPos(ImVec2{ 10, 620 });
	ImGui::SetNextWindowSize(ImVec2(350, 200));
	ImGui::Begin("Simulation Values", NULL, flags);
	ImGui::Text("Simulation run time");
	ImGui::Text("temperature map - capacity remap: %fms", remap_time);
	ImGui::Text("temperature map - update: %fms", weather_update_time);
	ImGui::Text("	wind update: %fms", sim_times[0]);
	ImGui::Text("	solar heat: %fms", sim_times[1]);
	ImGui::Text("	evap update: %fms", sim_times[2]);
	ImGui::Text("	cloud update: %fms", sim_times[3]);
	ImGui::Text("	ground to air update: %fms", sim_times[4]);
	ImGui::Text("	radiate update: %fms", sim_times[5]);
	ImGui::Text("	pressure gradient update: %fms", sim_times[6]);

	ImGui::End();
}

void Map::drawCreationMenu() {
	ImGuiWindowFlags flags = 0;
	flags |= ImGuiWindowFlags_NoMove;
	flags |= ImGuiWindowFlags_NoResize;
	flags |= ImGuiWindowFlags_NoCollapse;

	ImGui::SetNextWindowPos(ImVec2{ 10,10 });
	ImGui::SetNextWindowSize(ImVec2{ 300,700 });

	ImGui::Begin("Map Creation", NULL, flags);
	ImGui::SameLine(4, 0);
	ImGui::TextWrapped("The parameters below can be used to create a custom game map. Alternatively, a map can be generated using a preset algorithm with the \"Auto Generate\" button");
	ImGui::Text("");
	ImGui::SeparatorText("Heightmap parameters");

	ImGui::PushItemWidth(150);
	ImGui::SliderFloat("Noise Frequency", &perlin_freq, 0.0001f, 0.05f);
	ImGui::SameLine(); GUIutils::HoverTip("change noise level of detail");

	ImGui::PushItemWidth(150);
	ImGui::SliderFloat("Weight", &octave_weight, 0.0f, 1.0f);
	ImGui::SameLine(); GUIutils::HoverTip("change noise weight multiplier");

	//Fractal Choices
	if (ImGui::Button("Toggle Ridged Fractal Noise", ImVec2{ 200, 30 })) {
		fractal_ridge = !fractal_ridge;
	}
	ImGui::SameLine();
	if (fractal_ridge) {
		ImGui::Text("Active", ImVec2{ 40, 30 });
	}
	else {
		ImGui::Text("Inactive");
	}
	ImGui::SameLine(); GUIutils::HoverTip("activate or deactivate fractal step for noise generation");
	ImGui::Text("");

	//Add noise
	if (ImGui::Button("Add Noise", ImVec2{ 148, 30 })) {
		h_map.addNoise(perlin_freq, fractal_ridge, octave_weight);	//apply perlin noise
		updateBase();
	}
	ImGui::SameLine();
	//subtract noise
	if (ImGui::Button("Subtract Noise", ImVec2{ 148,30 })) {
		h_map.subNoise(perlin_freq, fractal_ridge, octave_weight);
		updateBase();
	}

	//refresh map button
	if (ImGui::Button("Reset Map", ImVec2{ 300,30 })) {
		//refresh internal maps
		h_map.refresh();
		t_map.refresh();
		tiles.refresh();
		sunx = 0;

		//reset graphical flags
		draw_surface_temp = false, draw_air_temp = false;

		//load textured tiles into base handle
		//loadTextures("D:\\Software and Tools\\C++\\T4x\\VS_WIN_devel\\resources\\TileSet32.png", base_texture_id);

		//update base vbo
		updateBase();
	}
	ImGui::Separator();
	if (ImGui::Button("Auto Generate", ImVec2{ 300,30 })) {
		autoGenerate();
		updateBase();
	}

	ImGui::SeparatorText("World parameters");
	ImGui::SliderFloat("Sea Level", &ocean_level, 0.0f, 1.0f);
	ImGui::SliderFloat("Mountain Height", &mountain_height, ocean_level, 1.0f);
	ImGui::Text(""); ImGui::Text("");
	ImGui::SeparatorText("Options");
	if (ImGui::Button("Continue", ImVec2{ 300,30 })) {

	}

	if (ImGui::Button("Save Height Map", ImVec2{ 300,30 })) {

	}

	ImGui::Text(""); ImGui::Text("");
	ImGui::SeparatorText("Resource parameters");

	ImGui::Text("");
	ImGui::SameLine(4,0);
	ImGui::Text("Abundance");
	ImGui::PushItemWidth(150); ImGui::SliderFloat("Iron", &iron_ab, 0.0f, 1.0f);
	ImGui::PushItemWidth(150); ImGui::SliderFloat("Chromium", &chrom_ab, 0.0f, 1.0f);
	ImGui::PushItemWidth(150); ImGui::SliderFloat("Copper", &copper_ab, 0.0f, 1.0f);
	ImGui::PushItemWidth(150); ImGui::SliderFloat("Oil", &oil_ab, 0.0f, 1.0f);
	ImGui::PushItemWidth(150); ImGui::SliderFloat("Coal", &coal_ab, 0.0f, 1.0f);

	ImGui::SameLine(); GUIutils::HoverTip("change noise level of detail");
	if (ImGui::Button("Generate Resources", ImVec2{ 300,30 })) {
		tiles.genResources(h_map.getHeightMap(), iron_ab, chrom_ab, copper_ab, oil_ab, coal_ab);
	}


	ImGui::End();
}