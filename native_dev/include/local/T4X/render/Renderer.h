#pragma once
// T4X includes
#include "T4X/render/RenderObjects/WorldObjects/MeshObject.h"
#include "T4X/render/RenderObjects/WorldObjects/TestObject.h"
#include "T4X/render/RenderObjects/TexturedObject.h"

#include "T4X/render/Menus/Menus.h"

#include "T4X/render/Camera.h"
#include "T4X/input/Event.h"

// cstd includes
#include <stdio.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <stack>
#include <memory>

// lib includes
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <glm/mat4x4.hpp>

// imgui includes
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>

// glm debug extensions
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>

class Renderer
{
private:
	/*
		Window Data and transform data
	*/
	// initial window values
	int WINDOW_W = 700;
	int WINDOW_H = 700;

	// transform matrix parameters
	glm::mat4 projection = glm::mat4(1.0f); // global projection matrix
	Camera camera;
	float fov = 45.0f;

	// lighting parameters
	glm::vec3 light_source = glm::vec3(0.0, 0.0, 1.0);
	float ambient_strength = 0.5;

	const float min_scale = 0.01f;    // minimum tranform value - controls max zoom out level
	const float max_scale = 1000.5f;  // max transform value - controls max zoom in
	float adjustment_factor_W = 1.0f; // width adjustment factor for max and min transform
	float adjustment_factor_H = 1.0f; // height ' '
	const float move_speed = 0.001f;  // base traversal speed

	glm::vec4 clear_color = glm::vec4(0.5f, 0.5f, 0.5f, 1.00f); // background color

	/*
		sdl pointers
	*/
	SDL_Window* main_window = NULL;  // sdl window pointer
	SDL_GLContext gl_context = NULL; // sdl-opengl context

	/*
		Render system flags
	*/
	bool initialised = false;   // initialisation flag
	bool vsync_enabled = false; // succesful vsync initialisation
	bool wireframe = false;     // wireframe mode
	std::function<void(Event)> event_handler;

	/*
		Render Objects
	*/
	std::vector<MeshObject> meshes;
	std::vector<TestObject> t_obj;

	//object maps
	std::unordered_map<int, std::shared_ptr<WorldObject>> world_objects;
	std::unordered_map<int, std::shared_ptr<RenderObject>> flat_objects;

	//menu stack
	std::stack<std::shared_ptr<Menu>> menu_stack;
	const uint logo_id = 0;

	/*
		transform matrix update methods
	*/
	void updateView();
	void updateProjection();

	void updateLightLoc();
	void updateAmbient();

public:
	// set callback
	void set_handler(std::function<void(Event)> event_handle) { event_handler = event_handle; }

	/*
		Initialise the rendering manager on startup
			Returns bool: false on initialisation error
	*/
	bool initialise(int screen_height, int screen_width);

	/*
		Menu calls
	*/
	bool addMenu(std::shared_ptr<Menu> menu);
	bool popMenu();
	bool menuActive();

	/*
		Generic object calls
	*/
	bool hideFlatObject(uint id);
	bool hideWorldObject(uint id);
	bool showFlatObject(uint id);
	bool showWorldObject(uint id);

	/*
		Mesh calls
	*/
	std::shared_ptr<WorldObject> addMeshObject(uint id);
	bool setMeshData(uint id, float* data, int width, int height);

	/*
		Textured object calls
	*/
	std::shared_ptr<RenderObject> addTexturedObject(uint id, const char* filename);
	std::shared_ptr<RenderObject> addTexturedObject(uint id);
	bool setTexture(uint id, const char* filename);

	/*
		Update render screen size parameters
	*/
	void setScreenSize(int width, int height);

	/*
		Draw active objects
	*/
	void render();

	/*
		Clean up memory and shutdown submodules
	*/
	void shutdown();

	/*
		Input and event updaters
	*/
	void keyUpdate(RENDER_KEY_STATE state);
	void eventUpdate(Event e);

	// C constructor
	Renderer() {}

	// C destructor
	~Renderer() {}
};