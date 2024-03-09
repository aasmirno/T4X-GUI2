#pragma once
#include "RenderObject.h"
#include "ShapeObject.h"

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <SDL2/SDL.h>
#include <GL/glew.h>

struct Vec4{
    float                                                     x, y, z, w;
    constexpr Vec4()                                        : x(0.0f), y(0.0f), z(0.0f), w(0.0f) { }
    constexpr Vec4(float _x, float _y, float _z, float _w)  : x(_x), y(_y), z(_z), w(_w) { }
};

struct ShaderProgram{
    // program id
    GLuint program_id = 0;
    // shader handles
    GLuint vertex_shader = 0;
    GLuint frag_shader = 0;
};

class Renderer{
private:
    /*
        Window Data
    */
    //initial window values
	int WINDOW_W = 1900;
	int WINDOW_H = 1030;

    Vec4 clear_color = Vec4(0.45f, 0.55f, 0.60f, 1.00f);	//background color

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
    std::vector<ShaderProgram> active_programs; // vector of active shader programs

    /*
        Create a gl shader from source and return its handle
            path: path to source file
            type: type of shader
                GL_VERTEX_SHADER
                GL_FRAGMENT_SHADER
                etc.
    */
    GLuint loadShader(std::string path, GLenum type);

    /*
        Create a shader program
    */
    bool createProgram();
public:
    /*
        Initialise the rendering manager on startup
            Returns bool: false on initialisation error
    */
    bool initialise();

    bool addRenderObject();

    /*
        Update the window if necessary and swap buffers
    */
    void render();

    //C constructor
    Renderer(){}

    //C destructor
    ~Renderer(){}
};