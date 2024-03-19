#pragma once
#include "RenderObjects/TileObject.h"

#include <stdio.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <glm/mat4x4.hpp>

class Renderer
{
private:
    /*
        Window Data and transform data
    */
    // initial window values
    int WINDOW_W = 600;
    int WINDOW_H = 600;

    //transform data
    glm::mat4 transform = glm::mat4(
        glm::vec4(0.05, 0.0, 0.0, 0.0),
        glm::vec4(0.0, 0.05, 0.0, 0.0),
        glm::vec4(0.0, 0.0, 1.0, 0.0),
        glm::vec4(0.0, 0.0, 0.0, 1.0));
    const float min_transform = 0.01f;
    const float max_transform = 1.0f;
    const float move_speed = 0.01f;


    glm::vec4 clear_color = glm::vec4(0.45f, 0.55f, 0.60f, 1.00f); // background color

    /*
        sdl pointers
    */
    SDL_Window *main_window = NULL;  // sdl window pointer
    SDL_GLContext gl_context = NULL; // sdl-opengl context

    /*
        System flags
    */
    bool initialised = false;   // initialisation flag
    bool vsync_enabled = false; // succesful vsync initialisation

    /*
        Render Objects
    */
    std::vector<TileObject> tile_objects;

public:
    /*
        Initialise the rendering manager on startup
            Returns bool: false on initialisation error
    */
    bool initialise();

    /*
        Create tiled texture object:
            x_dim: x dimension
            y_dim: y_dimension
            texture_source: path to texture file
            texture_w: texture file width
            texture_h: texture file height
    */
    TileObject *addTileObject(int x_dim, int y_dim,
                              uint16_t* data,
                              const char *texture_source, unsigned texture_w, unsigned texture_h);

    /*
        Update global transform
                
    */
    void adj_transform(int event_value);    //zoom in or out
    void move_transform(char dir);          //directional change

    /*
        Update the window if necessary and swap buffers
    */
    void render();

    /*
        Clean up memory and shutdown submodules
    */
    void shutdown();

    // C constructor
    Renderer() {}

    // C destructor
    ~Renderer() {}
};