#pragma once
#include "GlewLoader.h"

#include <stdio.h>
#include <iostream>
#include <vector>

#include <gl/glew.h>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif

/*
*	Implementation of main window and game instantiation
*/
class MainWindow {
private:
	SDL_Window* main_window;	//sdl window pointer
	SDL_GLContext gl_context;	//sdl-opengl context
	ImGuiContext* im_context;	//imgui context
	GlewLoader glew;	//glew shader loader

	GLint x = 0;
	GLint y = 0;
	GLint cam_rate = 5;

	ImVec4 clear_color;	//background color
	bool Running;	//run flag

private:
	bool Init();

	void Render();

	void Loop();

	void Cleanup();

	void EventHandle(SDL_Event& event);

public:
	MainWindow();

	int Start();

};
