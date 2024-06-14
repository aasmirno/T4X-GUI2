#pragma once
#include "RenderObjects/TexturedObject.h"
#include "RenderObjects/MeshObject.h"

#include <stdio.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <glm/mat4x4.hpp>

// glm debug extensions
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>

enum rDirection {
    RD_LEFT,
    RD_RIGHT
};

enum rAxis {
    RD_X,
    RD_Y,
    RD_Z
};

class Renderer
{
private:
    /*
        Window Data and transform data
    */
    // initial window values
    int WINDOW_W = 600;
    int WINDOW_H = 600;

    // transform data
    glm::mat4 transform = glm::mat4(
        glm::vec4(1.0, 0.0, 0.0, 0.0),  // col1
        glm::vec4(0.0, 1.0, 0.0, 0.0),  // col2
        glm::vec4(0.0, 0.0, 1.0, 0.0),  // col3
        glm::vec4(0.0, 0.0, 0.0, 1.0)); // col4: translation

    const float min_scale = 0.01f;    // minimum tranform value - controls max zoom out level
    const float max_scale = 100.5f;   // max transform value - controls max zoom in
    float adjustment_factor_W = 1.0f; // width adjustment factor for max and min transform
    float adjustment_factor_H = 1.0f; // height ' '
    const float move_speed = 0.001f;  // base traversal speed
    double rotation_angle = 1;
    const double cos_t = glm::cos(rotation_angle);
    const double sin_t = glm::sin(rotation_angle);


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

    void updateTransform();

public:
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
        object.setTransform(&transform[0][0]);
        // add it to active objects
        m_objects.push_back(object);
        return &m_objects.back();
    }

    /*
        Global transform update methods
    */
    void setScale(int event_value);              // zoom in or out
    void setTranslation(char direction);         // adjust translation
    void setRotation(rAxis axis, rDirection direction); // rotate

    void setScreenSize(int width, int height); // set screen size values and adjust viewport

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