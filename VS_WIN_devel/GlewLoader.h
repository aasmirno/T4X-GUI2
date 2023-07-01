#pragma once
#include <stdio.h>
#include <iostream>
#include <vector>

#include <gl/glew.h>
#include <gl/GLU.h>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif

class GlewLoader {
private:
	GLuint gProgramID = 0;
	GLint gVertexPos2DLocation = -1;
	GLuint gVBO = 0;
	GLuint gIBO = 0;
public:
	bool init();
	GLuint getID();
	GLuint getVBO();
	GLuint getIBO();
	GLint getVpos();
private:
	bool loadVertexShader();
	bool loadFragShader();
};