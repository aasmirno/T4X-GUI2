#pragma once

// T4X includes
#include "RenderObjects/TexturedObject.h"
#include "RenderObjects/MeshObject.h"
#include "Camera.h"

// cstd includes
#include <stdio.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>

// lib includes
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <glm/mat4x4.hpp>

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
    int WINDOW_W = 600;
    int WINDOW_H = 600;

    // transform matrix parameters
    glm::mat4 projection = glm::mat4(1.0f); // global projection matrix
    Camera camera;

    const float min_scale = 0.01f;    // minimum tranform value - controls max zoom out level
    const float max_scale = 1000.5f;  // max transform value - controls max zoom in
    float adjustment_factor_W = 1.0f; // width adjustment factor for max and min transform
    float adjustment_factor_H = 1.0f; // height ' '
    const float move_speed = 0.001f;  // base traversal speed

    glm::vec4 clear_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.00f); // background color

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
    std::vector<TexturedObject> texture_objects;
    std::vector<MeshObject> m_objects;

    /*
        transform matrix update methods
    */
    void updateView();
    void updateProjection();

public:
    void updateCamera(SDL_KeyCode key);

    /*
        Initialise the rendering manager on startup
            Returns bool: false on initialisation error
    */
    bool initialise(int screen_height, int screen_width);

    /*
        Create tiled texture object:
            texture_source: path to texture file
            texture_w: texture file width
            texture_h: texture file height
    */
    TexturedObject *addTexturedObject(const char *texture_source, unsigned texture_w, unsigned texture_h);
    MeshObject *aMO()
    {
        if (!initialised)
        {
            printf("ERROR: render manager not initialised\n");
            return nullptr;
        }

        MeshObject object;
        if (!object.initialise(1))
        {
            printf("Shape object initialisation failed\n");
            return nullptr;
        }

        // add it to active objects
        m_objects.push_back(object);
        updateProjection();
        updateView();

        return &m_objects.back();
    }

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

    // C constructor
    Renderer() {}

    // C destructor
    ~Renderer() {}
};