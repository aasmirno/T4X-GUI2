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
	SDL_Window* main_window;
	SDL_GLContext gl_context;
	ImGuiContext* im_context;

	ImVec4 clear_color;
	bool Running;
	GlewLoader glew;

private:
	bool Init();

	void Render();

	void Loop();

	void Cleanup();

public:
	MainWindow();

	int Start();

};
