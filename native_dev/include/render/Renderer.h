#pragma once
#include "RenderObjects/RenderObject.h"
#include "RenderObjects/ShapeObject.h"
#include "RenderObjects/TileObject.h"

#include "ShaderManager.h"

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <glm/mat4x4.hpp>

class Renderer{
private:
    /*
        Window Data and transform data
    */
    //initial window values
	int WINDOW_W = 600;
	int WINDOW_H = 600;

    glm::mat4 transform = glm::mat4(
        glm::vec4(0.166, 0.0, 0.0, 0.0),
        glm::vec4(0.0, 0.166, 0.0, 0.0),
        glm::vec4(0.0, 0.0, 1.0, 0.0),
        glm::vec4(0.0, 0.0, 0.0, 1.0)
    );

    glm::vec4 clear_color = glm::vec4(0.45f, 0.55f, 0.60f, 1.00f);	//background color

    /*
		sdl pointers
	*/
	SDL_Window* main_window = NULL;		//sdl window pointer
	SDL_GLContext gl_context = NULL;	//sdl-opengl context

    /*
        System flags
    */
    bool initialised = false;   //initialisation flag
    bool vsync_enabled = false; //succesful vsync initialisation
    
    /*
        Render Objects
    */
    std::vector<ShapeObject> active_objects; // vector of active render objects
    std::vector<TileObject> tile_objects;

    /*
        Shader values
    */
    std::vector<ShaderProgram> active_programs; // vector of active shader programs
    ShaderManager shader_manager;   //shader manager instance
public:
    /*
        Initialise the rendering manager on startup
            Returns bool: false on initialisation error
    */
    bool initialise();

    bool addRenderObject();
    bool addTileObject();

    void adj_transform(float factor);

    /*
        Update the window if necessary and swap buffers
    */
    void render();

    //C constructor
    Renderer(){}

    //C destructor
    ~Renderer(){}
};