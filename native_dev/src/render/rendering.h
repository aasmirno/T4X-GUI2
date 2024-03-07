#pragma once
#include <stdio.h>

#include <SDL2/SDL.h>
#include <GL/glu.h>

class Renderer{
    private:
    /*
        Metadata
    */
    //initial window values
	int WINDOW_W = 1900;
	int WINDOW_H = 1030;    

    /*
		sdl values
	*/
	SDL_Window* main_window = NULL;		//sdl window pointer
	SDL_GLContext gl_context = NULL;	//sdl-opengl context

    //C constructor
    Renderer(){}

    //C destructor
    ~Renderer(){}

    /*
        Initialise the rendering manager on startup
            Returns bool: false on error
    */
    bool initialise();

};